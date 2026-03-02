var builder = WebApplication.CreateBuilder(args);

// Razor Pages UI
builder.Services.AddRazorPages();

// Optional: HttpClient preconfigured to talk to your Api project
builder.Services.AddHttpClient("RunnerApi", client =>
{
    client.BaseAddress = new Uri("http://127.0.0.1:5166/");
});

var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error");
    app.UseHsts();

    // Only force HTTPS in non-dev (so http://127.0.0.1:5030 works in dev)
    app.UseHttpsRedirection();
}

app.UseRouting();
app.UseAuthorization();

// Proxy endpoint: browser -> Web (:5030) -> Api (:5166)
app.MapPost("/run-csharp", async (HttpContext ctx, IHttpClientFactory httpClientFactory) =>
{
    using var reader = new StreamReader(ctx.Request.Body);
    var code = await reader.ReadToEndAsync();

    var client = httpClientFactory.CreateClient("RunnerApi");
    using var resp = await client.PostAsync(
        "run-csharp",
        new StringContent(code, System.Text.Encoding.UTF8, "text/plain"));

    var body = await resp.Content.ReadAsStringAsync();

    ctx.Response.StatusCode = (int)resp.StatusCode; 
    ctx.Response.ContentType = resp.Content.Headers.ContentType?.ToString() ?? "text/plain";
    await ctx.Response.WriteAsync(body);
});

// Static files / assets
app.MapStaticAssets();
app.MapRazorPages().WithStaticAssets();

// Run the homepage on port 5030, and the API on port 5166 (see launchSettings.json in each project)
app.Run();