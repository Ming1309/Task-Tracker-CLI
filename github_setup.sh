#!/bin/bash

# Script to connect to GitHub repository
# Replace YOUR_USERNAME and YOUR_REPO_NAME with actual values

echo "🔗 Connecting to GitHub Repository..."
echo "======================================"

# Check if git is initialized
if [ ! -d ".git" ]; then
    echo "❌ Git not initialized. Run 'git init' first."
    exit 1
fi

# Prompt for GitHub repository URL
echo "📝 Please provide your GitHub repository URL:"
echo "   Format: https://github.com/username/repo-name.git"
echo "   Or SSH: git@github.com:username/repo-name.git"
echo ""
read -p "Repository URL: " REPO_URL

if [ -z "$REPO_URL" ]; then
    echo "❌ Repository URL cannot be empty."
    exit 1
fi

# Add remote origin
echo "🔄 Adding remote origin..."
git remote add origin "$REPO_URL"

# Verify remote was added
echo "✅ Remote origin added:"
git remote -v

# Set upstream and push
echo "🚀 Pushing to GitHub..."
git branch -M main
git push -u origin main

echo ""
echo "✅ Successfully connected to GitHub!"
echo "🌐 Your repository is now available at:"
echo "   $REPO_URL"
echo ""
echo "📋 Next steps:"
echo "   - Visit your repository on GitHub"
echo "   - Add repository description and topics"
echo "   - Set up GitHub Actions (optional)"
echo "   - Add collaborators (optional)"
