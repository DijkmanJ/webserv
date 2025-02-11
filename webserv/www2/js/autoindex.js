async function listFiles() {
    try {
        let currentPath = window.location.pathname;
        if (currentPath.endsWith('index.html')) {
            // Strip 'index.html' and get the parent directory
            currentPath = currentPath.slice(0, -10);  // Remove the last 10 characters (index.html)
        }
        document.querySelector('.directory-title').textContent = `Directory: ${currentPath}`;
        document.title = `Directory: ${currentPath}`;
        // Fetch the list of files in the directory
        const response = await fetch(`${currentPath}/list-files`);
        const text = await response.text();
        const files = JSON.parse(text);
        
        const fileList = document.getElementById('fileList');
        fileList.innerHTML = '';
        files.forEach(file => {
            const listItem = document.createElement('li');
            if (file.endsWith('.html') || file.endsWith('.png') || file.endsWith('.jpg')) {
                const link = document.createElement('a');
                link.href = currentPath + file;
                link.style.color = '#fff';
                link.style.textDecoration = 'none';
                link.style.display = 'block';
                link.style.width = '100%';
                
                // For images, open in a new tab
                if (file.endsWith('.png') || file.endsWith('.jpg')) {
                    link.target = '_blank';
                }
                const fileNameSpan = document.createElement('span');
                fileNameSpan.textContent = file;
                fileNameSpan.className = 'file-name';
                
                link.appendChild(fileNameSpan);
                listItem.appendChild(link);
            } 
            else {
                const fileNameSpan = document.createElement('span');
                fileNameSpan.textContent = file;
                fileNameSpan.className = 'file-name';
                listItem.appendChild(fileNameSpan);
            }
            fileList.appendChild(listItem);
        });
    } catch (error) {
        console.error('Error:', error);
    }
}
listFiles();