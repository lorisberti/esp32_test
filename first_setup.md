**Initialize git in the workspace:**

cd /d d:\Projects\Display_test\Display_test
git init

**Stage and commit your files:**
git add .
git commit -m "Initial commit"
Create a repository on GitHub

**Go to https://github.com/new
Name it, choose public/private, then create it
Add the GitHub remote and push:**

git remote add origin https://github.com/<your-user>/<repo-name>.git
git branch -M main
git push -u origin main