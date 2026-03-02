using System.Diagnostics;
using System.Text;

var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapPost("/run-csharp", async (HttpContext ctx) =>
{
    using var reader = new StreamReader(ctx.Request.Body, Encoding.UTF8);
    var code = await reader.ReadToEndAsync();

    if (code.Length > 20000)
        return Results.BadRequest("Too large");

    var userHome = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
    var baseRoot = Path.Combine(userHome, ".csharp-run");
    var root = Path.Combine(baseRoot, Guid.NewGuid().ToString("N"));
    var appDir = Path.Combine(root, "App");
    Directory.CreateDirectory(appDir);

    await File.WriteAllTextAsync(Path.Combine(appDir, "App.csproj"),
@"<Project Sdk=""Microsoft.NET.Sdk"">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
  </PropertyGroup>
</Project>");

    await File.WriteAllTextAsync(Path.Combine(appDir, "Program.cs"), code);

    var unconfined = Environment.GetEnvironmentVariable("RUNNER_UNCONFINED") == "1";

    var psi = new ProcessStartInfo
    {
        FileName = "docker",
        Arguments =
            $"run --rm -i " +
            $"-v \"{appDir}:/work/App\" -w /work/App " +
            $"csharp-runner " +
            $"dotnet run --project /work/App/App.csproj -c Release",
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        UseShellExecute = false
    };

    using var proc = Process.Start(psi)!;

    var finished = proc.WaitForExit(120 * 1000);
    if (!finished)
    {
        try { proc.Kill(true); } catch { }
        return Results.Problem("Timeout");
    }

    var stdout = await proc.StandardOutput.ReadToEndAsync();
    var stderr = await proc.StandardError.ReadToEndAsync();

    try { Directory.Delete(root, true); } catch { }

    return Results.Json(new { proc.ExitCode, stdout, stderr });
});

app.Run();