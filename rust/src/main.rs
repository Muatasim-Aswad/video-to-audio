mod config;
mod converter;
mod video_file;

use colored::*;
use converter::VideoConverter;

#[tokio::main]
async fn main() {
    // Set up ctrl+c handler
    tokio::spawn(async {
        tokio::signal::ctrl_c().await.ok();
        println!("\n{}", "👋 Goodbye!".blue());
        std::process::exit(0);
    });

    let converter = VideoConverter::new();

    if let Err(e) = converter.run().await {
        println!("{}", format!("❌ Fatal error: {}", e).red());
        std::process::exit(1);
    }
}
