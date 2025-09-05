use std::collections::HashMap;
use std::fs;
use std::path::Path;

/// Configuration settings for the video converter
#[derive(Debug, Clone)]
pub struct Config {
    pub default_dir: String,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            default_dir: "/Users/hackyourfuture/Downloads".to_string(),
        }
    }
}

impl Config {
    /// Load configuration from environment file
    pub fn load() -> Self {
        let mut config = Config::default();
        
        // Try to load .env file from parent directory
        let env_path = Path::new("../.env");
        
        if env_path.exists() {
            if let Ok(env_vars) = load_env_file(env_path) {
                if let Some(default_dir) = env_vars.get("DEFAULT_DIR") {
                    config.default_dir = default_dir.trim().to_string();
                }
            }
        }
        
        config
    }
}

/// Parse .env file and return key-value pairs
fn load_env_file(path: &Path) -> Result<HashMap<String, String>, std::io::Error> {
    let content = fs::read_to_string(path)?;
    let mut env_vars = HashMap::new();
    
    for line in content.lines() {
        let line = line.trim();
        if line.is_empty() || line.starts_with('#') || !line.contains('=') {
            continue;
        }
        
        if let Some((key, value)) = line.split_once('=') {
            env_vars.insert(key.trim().to_string(), value.trim().to_string());
        }
    }
    
    Ok(env_vars)
}
