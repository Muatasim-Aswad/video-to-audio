import ffmpeg from "fluent-ffmpeg";
import path from "path";
import { fileURLToPath } from "url";
import fs from "fs";
import { input, confirm, select } from "@inquirer/prompts";

// Get __dirname equivalent in ES modules
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const DEFAULT_DIR = process.env.DEFAULT_DIR || "/";

// Get all supported video files in a directory
function getVideoFiles(directory) {
  try {
    if (!fs.existsSync(directory)) {
      return [];
    }

    const supportedFormats = /\.(mp4|avi|mov|mkv|flv|wmv|webm|m4v|3gp)$/i;

    const files = fs
      .readdirSync(directory)
      .filter((file) => {
        const fullPath = path.join(directory, file);
        const stats = fs.statSync(fullPath);
        return stats.isFile() && supportedFormats.test(file);
      })
      .map((file) => {
        const fullPath = path.join(directory, file);
        const stats = fs.statSync(fullPath);
        const fileSizeMB = (stats.size / (1024 * 1024)).toFixed(1);
        const extension = path.extname(file).toLowerCase();

        return {
          name: `${file} (${fileSizeMB} MB)`,
          value: file,
          description: `${extension} video file`,
        };
      })
      .sort((a, b) => a.value.localeCompare(b.value));

    return files;
  } catch (error) {
    console.error(`Error reading directory: ${error.message}`);
    return [];
  }
}

// Function to check if input is a URL
function isUrl(string) {
  try {
    new URL(string);
    return true;
  } catch (_) {
    return false;
  }
}

// Function to get output path based on input
function getOutputPath(inputPath) {
  const parsedPath = path.parse(inputPath);

  // If it's a URL, use the pathname
  if (isUrl(inputPath)) {
    const url = new URL(inputPath);
    const urlPath = path.parse(url.pathname);
    return path.join(__dirname, `${urlPath.name || "output"}.mp3`);
  }

  // For local files, save in the same directory with .mp3 extension
  return path.join(parsedPath.dir || __dirname, `${parsedPath.name}.mp3`);
}

// Convert video to MP3
function convertToMp3(inputFile, outputFile) {
  return new Promise((resolve, reject) => {
    console.log(`🔄 Converting: ${path.basename(inputFile)} → ${path.basename(outputFile)}`);

    ffmpeg(inputFile)
      .output(outputFile)
      .audioCodec("libmp3lame")
      .audioBitrate("192k") // Good quality audio
      .on("start", (commandLine) => {
        console.log(
          "🎬 FFmpeg started with command:",
          commandLine.split(" ").slice(0, 3).join(" ") + "...",
        );
      })
      .on("progress", (progress) => {
        if (progress.percent) {
          process.stdout.write(`\r⏳ Progress: ${Math.round(progress.percent)}%`);
        }
      })
      .on("end", () => {
        console.log(`\n✅ Conversion finished: ${outputFile}`);
        resolve();
      })
      .on("error", (err) => {
        console.error("\n❌ Error:", err.message);
        reject(err);
      })
      .run();
  });
}

// Main function
async function main() {
  try {
    console.log("🎵 Video to Audio Converter");
    console.log("===========================");

    // Ask about default directory
    console.log(`📁 Default directory: ${DEFAULT_DIR}`);
    const useDefault = await confirm({
      message: "Use this as the root directory?",
      default: true,
    });

    let rootDir = DEFAULT_DIR;
    if (!useDefault) {
      rootDir = await input({
        message: "Enter root directory path:",
        validate: (path) => {
          if (!fs.existsSync(path)) {
            return `Directory not found: ${path}`;
          }
          return true;
        },
      });
    }

    console.log(`\n🗂️  Working in: ${rootDir}`);

    // Get all video files in the directory
    const videoFiles = getVideoFiles(rootDir);
    let selectedFile;

    if (videoFiles.length === 0) {
      console.log("❌ No supported video files found in the directory.");
      console.log("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp");

      // Offer manual input as fallback
      const manualInput = await confirm({
        message: "Would you like to enter a file path or URL manually?",
        default: true,
      });

      if (!manualInput) {
        console.log("👋 Goodbye!");
        return;
      }

      selectedFile = await input({
        message: "Enter video file path or URL:",
        validate: (input) => {
          if (!input.trim()) {
            return "Please enter a file path or URL";
          }
          return true;
        },
      });
    } else {
      console.log(`📹 Found ${videoFiles.length} video file(s)`);

      // Add manual input option
      const choices = [
        ...videoFiles,
        {
          name: "📝 Enter file path or URL manually",
          value: "MANUAL_INPUT",
          description: "Type a custom path or URL",
        },
      ];

      const selection = await select({
        message: "Select a video file to convert:",
        choices: choices,
        pageSize: 10,
      });

      if (selection === "MANUAL_INPUT") {
        selectedFile = await input({
          message: "Enter video file path or URL:",
          validate: (input) => {
            if (!input.trim()) {
              return "Please enter a file path or URL";
            }
            return true;
          },
        });
      } else {
        selectedFile = selection;
      }
    }

    let fullInputPath = selectedFile;

    // If it's not a URL and not an absolute path, assume it's in the root directory
    if (!isUrl(selectedFile) && !path.isAbsolute(selectedFile)) {
      fullInputPath = path.join(rootDir, selectedFile);
    }

    // Check if local file exists
    if (!isUrl(fullInputPath) && !fs.existsSync(fullInputPath)) {
      console.log(`❌ File not found: ${fullInputPath}`);
      return;
    }

    const output = getOutputPath(fullInputPath);

    console.log(`\n📁 Input: ${fullInputPath}`);
    console.log(`💾 Output: ${output}`);
    console.log();

    await convertToMp3(fullInputPath, output);

    // Check if output file was created
    if (fs.existsSync(output)) {
      const stats = fs.statSync(output);
      const fileSizeMB = (stats.size / (1024 * 1024)).toFixed(2);
      console.log(`📊 Output file size: ${fileSizeMB} MB`);
    }
  } catch (error) {
    console.error("❌ Conversion failed:", error.message);
  }
}

// Run the main function
main();
