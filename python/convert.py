#!/usr/bin/env python3
"""
Video to Audio Converter (Python)
Converts video files to MP3 format using FFmpeg
"""

import os
import sys
import subprocess
import urllib.parse
from pathlib import Path
from typing import List, Optional, Dict, Any


class VideoFile:
    """Represents a video file with metadata"""
    
    def __init__(self, name: str, path: str, size_mb: float, extension: str):
        self.name = name
        self.path = path
        self.size_mb = size_mb
        self.extension = extension
    
    def __str__(self) -> str:
        return f"{self.name} ({self.size_mb:.1f} MB)"


class VideoConverter:
    """Main video converter class"""
    
    SUPPORTED_EXTENSIONS = {'.mp4', '.avi', '.mov', '.mkv', '.flv', '.wmv', '.webm', '.m4v', '.3gp'}
    
    def __init__(self):
        self.config = self._load_config()
    
    def _load_config(self) -> Dict[str, str]:
        """Load configuration from .env file"""
        config = {
            'default_dir': '/Users/hackyourfuture/Downloads'  # fallback default
        }
        
        # Try to load .env file from parent directory
        env_path = Path(__file__).parent.parent / '.env'
        if env_path.exists():
            try:
                with open(env_path, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and '=' in line:
                            key, value = line.split('=', 1)
                            if key.strip() == 'DEFAULT_DIR':
                                config['default_dir'] = value.strip()
            except Exception:
                pass
        
        return config
    
    @staticmethod
    def is_url(string: str) -> bool:
        """Check if a string is a valid URL"""
        try:
            result = urllib.parse.urlparse(string)
            return all([result.scheme, result.netloc])
        except Exception:
            return False
    
    def get_video_files(self, directory: str) -> List[VideoFile]:
        """Scan directory for supported video files"""
        video_files = []
        
        try:
            directory_path = Path(directory)
            if not directory_path.exists():
                return []
            
            for file_path in directory_path.iterdir():
                if file_path.is_file() and file_path.suffix.lower() in self.SUPPORTED_EXTENSIONS:
                    size_mb = file_path.stat().st_size / (1024 * 1024)
                    
                    video_files.append(VideoFile(
                        name=file_path.name,
                        path=str(file_path),
                        size_mb=size_mb,
                        extension=file_path.suffix.lower()
                    ))
            
            # Sort files alphabetically
            video_files.sort(key=lambda x: x.name.lower())
            
        except Exception as e:
            print(f"❌ Error reading directory: {e}")
            return []
        
        return video_files
    
    def get_output_path(self, input_path: str) -> str:
        """Generate output MP3 path based on input path"""
        if self.is_url(input_path):
            parsed_url = urllib.parse.urlparse(input_path)
            base_name = Path(parsed_url.path).stem or 'output'
            return f"{base_name}.mp3"
        
        input_path_obj = Path(input_path)
        return str(input_path_obj.with_suffix('.mp3'))
    
    def convert_to_mp3(self, input_path: str, output_path: str) -> bool:
        """Convert video to MP3 using ffmpeg"""
        print(f"🔄 Converting: {Path(input_path).name} → {Path(output_path).name}")
        
        try:
            # Build ffmpeg command
            cmd = [
                'ffmpeg',
                '-i', input_path,
                '-vn',  # no video
                '-acodec', 'libmp3lame',
                '-ab', '192k',  # audio bitrate
                '-y',  # overwrite output files
                output_path
            ]
            
            print("🎬 FFmpeg started...")
            
            # Run ffmpeg command with live output
            process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                universal_newlines=True
            )
            
            # Print ffmpeg progress
            for line in process.stdout:
                if 'time=' in line and 'bitrate=' in line:
                    # Extract and show progress
                    parts = line.strip().split()
                    time_part = next((part for part in parts if part.startswith('time=')), '')
                    if time_part:
                        print(f"\r⏳ Progress: {time_part.replace('time=', '')}", end='', flush=True)
            
            process.wait()
            print()  # New line after progress
            
            if process.returncode == 0:
                return True
            else:
                print(f"❌ FFmpeg failed with return code: {process.returncode}")
                return False
            
        except FileNotFoundError:
            print("❌ FFmpeg not found. Please make sure FFmpeg is installed and in your PATH.")
            return False
        except Exception as e:
            print(f"❌ Error during conversion: {e}")
            return False
    
    def get_file_size_mb(self, file_path: str) -> Optional[float]:
        """Get file size in MB"""
        try:
            return Path(file_path).stat().st_size / (1024 * 1024)
        except Exception:
            return None
    
    def ask_yes_no(self, question: str, default: bool = True) -> bool:
        """Ask a yes/no question"""
        default_str = "Y/n" if default else "y/N"
        while True:
            response = input(f"{question} ({default_str}): ").strip().lower()
            if not response:
                return default
            if response in ('y', 'yes'):
                return True
            if response in ('n', 'no'):
                return False
            print("Please enter 'y' or 'n'")
    
    def ask_directory(self) -> str:
        """Ask user for working directory"""
        print(f"📁 Default directory: {self.config['default_dir']}")
        
        if self.ask_yes_no("Use this as the root directory?", True):
            return self.config['default_dir']
        
        # Ask for custom directory
        while True:
            custom_dir = input("Enter root directory path: ").strip()
            if Path(custom_dir).exists():
                return custom_dir
            else:
                print(f"❌ Directory not found: {custom_dir}")
    
    def select_video_file(self, video_files: List[VideoFile], root_dir: str) -> str:
        """Let user select a video file or enter manual input"""
        if not video_files:
            print("❌ No supported video files found in the directory.")
            print("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp")
            
            if not self.ask_yes_no("Would you like to enter a file path or URL manually?", True):
                print("👋 Goodbye!")
                sys.exit(0)
            
            while True:
                file_input = input("Enter video file path or URL: ").strip()
                if file_input:
                    return file_input
                print("Please enter a file path or URL")
        
        print(f"📹 Found {len(video_files)} video file(s)")
        print("\nSelect a video file to convert:")
        
        # Show numbered list
        for i, video_file in enumerate(video_files, 1):
            print(f"  {i:2d}. {video_file}")
        
        print(f"  {len(video_files) + 1:2d}. 📝 Enter file path or URL manually")
        
        while True:
            try:
                choice = input(f"\nEnter choice (1-{len(video_files) + 1}): ").strip()
                choice_num = int(choice)
                
                if 1 <= choice_num <= len(video_files):
                    return video_files[choice_num - 1].name
                elif choice_num == len(video_files) + 1:
                    # Manual input
                    while True:
                        file_input = input("Enter video file path or URL: ").strip()
                        if file_input:
                            return file_input
                        print("Please enter a file path or URL")
                else:
                    print(f"Please enter a number between 1 and {len(video_files) + 1}")
            
            except ValueError:
                print("Please enter a valid number")
            except KeyboardInterrupt:
                print("\n👋 Goodbye!")
                sys.exit(0)
    
    def run(self):
        """Main application loop"""
        print("🎵 Video to Audio Converter (Python)")
        print("=====================================")
        
        try:
            # Get working directory
            root_dir = self.ask_directory()
            print(f"\n🗂️  Working in: {root_dir}")
            
            # Get video files in directory
            video_files = self.get_video_files(root_dir)
            
            # Let user select file
            selected_file = self.select_video_file(video_files, root_dir)
            
            # Determine full input path
            if self.is_url(selected_file) or Path(selected_file).is_absolute():
                full_input_path = selected_file
            else:
                full_input_path = str(Path(root_dir) / selected_file)
            
            # Check if local file exists
            if not self.is_url(full_input_path) and not Path(full_input_path).exists():
                print(f"❌ File not found: {full_input_path}")
                return
            
            # Generate output path
            output_path = self.get_output_path(full_input_path)
            
            print(f"\n📁 Input: {full_input_path}")
            print(f"💾 Output: {output_path}")
            print()
            
            # Perform conversion
            if self.convert_to_mp3(full_input_path, output_path):
                print(f"✅ Conversion finished: {output_path}")
                
                # Show output file size
                if size_mb := self.get_file_size_mb(output_path):
                    print(f"📊 Output file size: {size_mb:.2f} MB")
            else:
                print("❌ Conversion failed!")
                sys.exit(1)
        
        except KeyboardInterrupt:
            print("\n👋 Goodbye!")
            sys.exit(0)


def main():
    """Entry point"""
    try:
        converter = VideoConverter()
        converter.run()
    except KeyboardInterrupt:
        print("\n👋 Goodbye!")
        sys.exit(0)
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
