using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Spectre.Console;

namespace VideoToAudioConverter;

/// <summary>
/// Represents a video file with metadata
/// </summary>
public record VideoFile(string Name, string Path, double SizeMB, string Extension)
{
  public override string ToString() => $"{Name} ({SizeMB:F1} MB)";
}

/// <summary>
/// Configuration settings for the application
/// </summary>
public class Config
{
  public string DefaultDir { get; set; } = "/Users/hackyourfuture/Downloads";
}

/// <summary>
/// Main video converter class
/// </summary>
public class VideoConverter
{
  private static readonly string[] SupportedExtensions =
  {
        ".mp4", ".avi", ".mov", ".mkv", ".flv", ".wmv", ".webm", ".m4v", ".3gp"
    };

  private readonly Config _config;

  public VideoConverter()
  {
    _config = LoadConfig();
  }

  /// <summary>
  /// Load configuration from .env file
  /// </summary>
  private static Config LoadConfig()
  {
    var config = new Config();

    try
    {
      // Try to load .env file from parent directory
      var envPath = Path.Combine("..", "..", "..", "..", ".env");

      if (File.Exists(envPath))
      {
        var lines = File.ReadAllLines(envPath);
        foreach (var line in lines)
        {
          var trimmedLine = line.Trim();
          if (string.IsNullOrEmpty(trimmedLine) || !trimmedLine.Contains('='))
            continue;

          var parts = trimmedLine.Split('=', 2);
          if (parts.Length == 2 && parts[0].Trim() == "DEFAULT_DIR")
          {
            config.DefaultDir = parts[1].Trim();
          }
        }
      }
    }
    catch
    {
      // Ignore errors and use default
    }

    return config;
  }

  /// <summary>
  /// Check if a string is a valid URL
  /// </summary>
  private static bool IsUrl(string input)
  {
    return Uri.TryCreate(input, UriKind.Absolute, out var uri) &&
           (uri.Scheme == Uri.UriSchemeHttp || uri.Scheme == Uri.UriSchemeHttps);
  }

  /// <summary>
  /// Get all video files in a directory
  /// </summary>
  private static List<VideoFile> GetVideoFiles(string directory)
  {
    var videoFiles = new List<VideoFile>();

    try
    {
      if (!Directory.Exists(directory))
        return videoFiles;

      var files = Directory.GetFiles(directory)
          .Where(file => SupportedExtensions.Contains(Path.GetExtension(file).ToLower()))
          .Select(file =>
          {
            var fileInfo = new FileInfo(file);
            var sizeMB = fileInfo.Length / (1024.0 * 1024.0);

            return new VideoFile(
                      fileInfo.Name,
                      file,
                      sizeMB,
                      fileInfo.Extension.ToLower()
                  );
          })
          .OrderBy(vf => vf.Name, StringComparer.OrdinalIgnoreCase)
          .ToList();

      videoFiles.AddRange(files);
    }
    catch (Exception ex)
    {
      AnsiConsole.MarkupLine($"[red]❌ Error reading directory: {ex.Message}[/]");
    }

    return videoFiles;
  }

  /// <summary>
  /// Generate output MP3 path based on input path
  /// </summary>
  private static string GetOutputPath(string inputPath)
  {
    if (IsUrl(inputPath))
    {
      var uri = new Uri(inputPath);
      var baseName = Path.GetFileNameWithoutExtension(uri.LocalPath);
      return string.IsNullOrEmpty(baseName) ? "output.mp3" : $"{baseName}.mp3";
    }

    return Path.ChangeExtension(inputPath, ".mp3");
  }

  /// <summary>
  /// Convert video to MP3 using ffmpeg
  /// </summary>
  private static async Task<bool> ConvertToMp3Async(string inputPath, string outputPath)
  {
    AnsiConsole.MarkupLine($"🔄 Converting: [blue]{Path.GetFileName(inputPath)}[/] → [green]{Path.GetFileName(outputPath)}[/]");

    try
    {
      var processStartInfo = new ProcessStartInfo
      {
        FileName = "ffmpeg",
        Arguments = $"-i \"{inputPath}\" -vn -acodec libmp3lame -ab 192k -y \"{outputPath}\"",
        RedirectStandardError = true,
        RedirectStandardOutput = true,
        UseShellExecute = false,
        CreateNoWindow = true
      };

      AnsiConsole.MarkupLine("🎬 FFmpeg started...");

      await AnsiConsole.Progress()
          .AutoRefresh(false)
          .Columns(new ProgressColumn[]
          {
                    new TaskDescriptionColumn(),
                    new ProgressBarColumn(),
                    new PercentageColumn(),
                    new SpinnerColumn(),
          })
          .StartAsync(async ctx =>
          {
            var task = ctx.AddTask("Converting video to audio");

            using var process = Process.Start(processStartInfo);
            if (process == null)
            {
              throw new InvalidOperationException("Failed to start FFmpeg process");
            }

            var errorOutput = await process.StandardError.ReadToEndAsync();
            await process.WaitForExitAsync();

            // Update progress to complete
            task.Value = 100;
            ctx.Refresh();

            if (process.ExitCode != 0)
            {
              AnsiConsole.MarkupLine($"[red]❌ FFmpeg failed with exit code: {process.ExitCode}[/]");
              if (!string.IsNullOrEmpty(errorOutput))
              {
                AnsiConsole.MarkupLine($"[red]Error details: {errorOutput}[/]");
              }
              return false;
            }

            return true;
          });

      return true;
    }
    catch (System.ComponentModel.Win32Exception)
    {
      AnsiConsole.MarkupLine("[red]❌ FFmpeg not found. Please make sure FFmpeg is installed and in your PATH.[/]");
      return false;
    }
    catch (Exception ex)
    {
      AnsiConsole.MarkupLine($"[red]❌ Error during conversion: {ex.Message}[/]");
      return false;
    }
  }

  /// <summary>
  /// Get file size in MB
  /// </summary>
  private static double? GetFileSizeMB(string filePath)
  {
    try
    {
      var fileInfo = new FileInfo(filePath);
      return fileInfo.Length / (1024.0 * 1024.0);
    }
    catch
    {
      return null;
    }
  }

  /// <summary>
  /// Ask user for working directory
  /// </summary>
  private string AskDirectory()
  {
    AnsiConsole.MarkupLine($"📁 Default directory: [blue]{_config.DefaultDir}[/]");

    if (AnsiConsole.Confirm("Use this as the root directory?", true))
    {
      return _config.DefaultDir;
    }

    while (true)
    {
      var customDir = AnsiConsole.Ask<string>("Enter root directory path:");

      if (Directory.Exists(customDir))
      {
        return customDir;
      }

      AnsiConsole.MarkupLine($"[red]❌ Directory not found: {customDir}[/]");
    }
  }

  /// <summary>
  /// Let user select a video file or enter manual input
  /// </summary>
  private string SelectVideoFile(List<VideoFile> videoFiles, string rootDir)
  {
    if (videoFiles.Count == 0)
    {
      AnsiConsole.MarkupLine("[red]❌ No supported video files found in the directory.[/]");
      AnsiConsole.MarkupLine("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp");

      if (!AnsiConsole.Confirm("Would you like to enter a file path or URL manually?", true))
      {
        AnsiConsole.MarkupLine("👋 Goodbye!");
        Environment.Exit(0);
      }

      return AnsiConsole.Ask<string>("Enter video file path or URL:");
    }

    AnsiConsole.MarkupLine($"📹 Found [green]{videoFiles.Count}[/] video file(s)");

    var choices = videoFiles.Select(vf => vf.ToString()).ToList();
    choices.Add("📝 Enter file path or URL manually");

    var selection = AnsiConsole.Prompt(
        new SelectionPrompt<string>()
            .Title("Select a video file to convert:")
            .PageSize(10)
            .AddChoices(choices));

    if (selection == "📝 Enter file path or URL manually")
    {
      return AnsiConsole.Ask<string>("Enter video file path or URL:");
    }

    // Find the selected video file
    var selectedVideoFile = videoFiles.FirstOrDefault(vf => vf.ToString() == selection);
    return selectedVideoFile?.Name ?? string.Empty;
  }

  /// <summary>
  /// Main application logic
  /// </summary>
  public async Task RunAsync()
  {
    AnsiConsole.Write(
        new FigletText("Video to Audio")
            .LeftJustified()
            .Color(Color.Blue));

    AnsiConsole.MarkupLine("🎵 [bold]Video to Audio Converter (C#)[/]");
    AnsiConsole.MarkupLine("=====================================");

    try
    {
      // Get working directory
      var rootDir = AskDirectory();
      AnsiConsole.MarkupLine($"\n🗂️  Working in: [blue]{rootDir}[/]");

      // Get video files in directory
      var videoFiles = GetVideoFiles(rootDir);

      // Let user select file
      var selectedFile = SelectVideoFile(videoFiles, rootDir);

      // Determine full input path
      string fullInputPath;
      if (IsUrl(selectedFile) || Path.IsPathFullyQualified(selectedFile))
      {
        fullInputPath = selectedFile;
      }
      else
      {
        fullInputPath = Path.Combine(rootDir, selectedFile);
      }

      // Check if local file exists
      if (!IsUrl(fullInputPath) && !File.Exists(fullInputPath))
      {
        AnsiConsole.MarkupLine($"[red]❌ File not found: {fullInputPath}[/]");
        return;
      }

      // Generate output path
      var outputPath = GetOutputPath(fullInputPath);

      AnsiConsole.MarkupLine($"\n📁 Input: [blue]{fullInputPath}[/]");
      AnsiConsole.MarkupLine($"💾 Output: [green]{outputPath}[/]");
      AnsiConsole.WriteLine();

      // Perform conversion
      if (await ConvertToMp3Async(fullInputPath, outputPath))
      {
        AnsiConsole.MarkupLine($"✅ [green]Conversion finished: {outputPath}[/]");

        // Show output file size
        var sizeMB = GetFileSizeMB(outputPath);
        if (sizeMB.HasValue)
        {
          AnsiConsole.MarkupLine($"📊 Output file size: [yellow]{sizeMB:F2} MB[/]");
        }
      }
      else
      {
        AnsiConsole.MarkupLine("[red]❌ Conversion failed![/]");
        Environment.Exit(1);
      }
    }
    catch (Exception ex)
    {
      AnsiConsole.MarkupLine($"[red]❌ Unexpected error: {ex.Message}[/]");
      Environment.Exit(1);
    }
  }
}

/// <summary>
/// Program entry point
/// </summary>
public class Program
{
  public static async Task Main(string[] args)
  {
    try
    {
      var converter = new VideoConverter();
      await converter.RunAsync();
    }
    catch (OperationCanceledException)
    {
      AnsiConsole.MarkupLine("\n👋 Goodbye!");
    }
    catch (Exception ex)
    {
      AnsiConsole.MarkupLine($"[red]❌ Fatal error: {ex.Message}[/]");
      Environment.Exit(1);
    }
  }
}
