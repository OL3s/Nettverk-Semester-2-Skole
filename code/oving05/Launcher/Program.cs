using System.Diagnostics;
using System.Net.Http;

static async Task PumpToLogAsync(StreamReader reader, string logPath, string prefix, bool mirrorToConsole, CancellationToken ct)
{
    Directory.CreateDirectory(Path.GetDirectoryName(logPath)!);

    await using var fs = new FileStream(logPath, FileMode.Append, FileAccess.Write, FileShare.ReadWrite);
    await using var sw = new StreamWriter(fs) { AutoFlush = true };

    while (!ct.IsCancellationRequested)
    {
        var line = await reader.ReadLineAsync();
        if (line is null) break;

        var stamped = $"{DateTimeOffset.Now:HH:mm:ss} {prefix}{line}";
        await sw.WriteLineAsync(stamped);

        if (mirrorToConsole)
            Console.WriteLine(stamped);
    }
}

static Process StartDotnetRun(string projectPath, string aspnetcoreUrls)
{
    var psi = new ProcessStartInfo
    {
        FileName = "dotnet",
        Arguments = $"run --project \"{projectPath}\"",
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        UseShellExecute = false,
    };
    psi.Environment["ASPNETCORE_URLS"] = aspnetcoreUrls;

    var p = new Process { StartInfo = psi, EnableRaisingEvents = true };

    if (!p.Start())
        throw new Exception($"Failed to start: {projectPath}");

    return p;
}

static async Task WaitUntilUpAsync(string url, TimeSpan timeout)
{
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(2) };
    var sw = Stopwatch.StartNew();

    while (sw.Elapsed < timeout)
    {
        try
        {
            using var resp = await http.GetAsync(url);
            if ((int)resp.StatusCode < 500) return; // 200/302/404 are "up enough"
        }
        catch
        {
            // ignore while starting
        }

        await Task.Delay(300);
    }

    throw new TimeoutException($"Timed out waiting for {url}");
}

static void TryKill(Process? p)
{
    if (p is null) return;
    try
    {
        if (!p.HasExited) p.Kill(entireProcessTree: true);
    }
    catch { }
}

var verbose = args.Contains("--verbose");

// Change these if you want different ports:
var apiUrl = "http://127.0.0.1:3030";
var webUrl = "http://127.0.0.1:5030";

var root = "/home/ole-kristian-wigum/Documents/School/Nettverk/code/oving05";
var apiProject = Path.Combine(root, "Api", "Api.csproj");
var webProject = Path.Combine(root, "Web", "Web.csproj");

var logDir = Path.Combine(root, "LauncherLogs");
var apiLog = Path.Combine(logDir, "api.log");
var webLog = Path.Combine(logDir, "web.log");

Process? api = null;
Process? web = null;

using var cts = new CancellationTokenSource();

Console.CancelKeyPress += (_, e) =>
{
    e.Cancel = true;
    Console.WriteLine("Stopping...");
    cts.Cancel();
    TryKill(web);
    TryKill(api);
    Environment.Exit(0);
};

try
{
    Console.WriteLine("Starting API...");
    api = StartDotnetRun(apiProject, apiUrl);

    _ = Task.Run(() => PumpToLogAsync(api.StandardOutput, apiLog, "[API] ", verbose, cts.Token));
    _ = Task.Run(() => PumpToLogAsync(api.StandardError, apiLog, "[API][ERR] ", verbose, cts.Token));

    Console.WriteLine("Starting Web...");
    web = StartDotnetRun(webProject, webUrl);

    _ = Task.Run(() => PumpToLogAsync(web.StandardOutput, webLog, "[WEB] ", verbose, cts.Token));
    _ = Task.Run(() => PumpToLogAsync(web.StandardError, webLog, "[WEB][ERR] ", verbose, cts.Token));

    Console.WriteLine("Waiting for Web...");
    await WaitUntilUpAsync(webUrl, TimeSpan.FromSeconds(30));

    // Small delay so startup noise goes to the log first
    await Task.Delay(800, cts.Token);

    Console.WriteLine();
    Console.WriteLine($"Homepage: {webUrl}");
    Console.WriteLine($"API:      {apiUrl}");
    Console.WriteLine($"Logs:     {logDir}  (api.log / web.log)");
    Console.WriteLine("Press Ctrl+C to stop.");

    await Task.Delay(Timeout.InfiniteTimeSpan, cts.Token);
}
finally
{
    cts.Cancel();
    TryKill(web);
    TryKill(api);
}