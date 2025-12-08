#!/bin/bash

# YouTube Music Player - API Key Setup Script
# This script helps you configure your YouTube API key

echo "========================================="
echo "YouTube Music Player - API Key Setup"
echo "========================================="
echo ""

# Check if widget.cpp exists
if [ ! -f "widget.cpp" ]; then
    echo "Error: widget.cpp not found!"
    echo "Please run this script from the project root directory."
    exit 1
fi

echo "Please follow these steps to get your YouTube API Key:"
echo ""
echo "1. Go to: https://console.cloud.google.com/"
echo "2. Create a new project or select an existing one"
echo "3. Enable 'YouTube Data API v3'"
echo "4. Create credentials (API Key)"
echo "5. Copy your API key"
echo ""
echo "For detailed instructions, see USER_GUIDE.md"
echo ""

# Prompt for API key
read -p "Enter your YouTube API Key: " api_key

if [ -z "$api_key" ]; then
    echo "Error: API Key cannot be empty!"
    exit 1
fi

# Backup original file
cp widget.cpp widget.cpp.backup

# Replace the API key in widget.cpp
# Use a more specific pattern that matches the constructor initialization
sed -i "s/apiKey(\"YOUR_YOUTUBE_API_KEY_HERE\")/apiKey(\"$api_key\")/" widget.cpp

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ API Key has been configured successfully!"
    echo "✓ Original file backed up to widget.cpp.backup"
    echo ""
    echo "Next steps:"
    echo "1. Build the project: qmake6 last-report.pro && make"
    echo "2. Run the application: ./last-report"
    echo ""
else
    echo "Error: Failed to update widget.cpp"
    # Restore backup
    mv widget.cpp.backup widget.cpp
    exit 1
fi
