// Functie om de lijst van bestanden op te halen van de server

document.getElementById('uploadForm').addEventListener('submit', function (event) {
    event.preventDefault(); // voorkom de standaard formulierverzending

    const formData = new FormData(this);

    // Verstuur het bestand naar de server via fetch API
    fetch('/www2/uploads/', {method: 'POST', body: formData,})
    .then(response => 
    {
    if (response.ok)
        window.location.reload();
    else if (response.status === 413 ) 
        alert('413 Payload Too Large');
    else if (response.status === 405)
        alert('405 Method Not Allowed');
    else if (response.status === 415)
        alert('415 Unsupported Media Type');
    })
    .catch(error => {
      console.error('Error:', error);
      alert('Error uploading file');
    });
  });

async function listFiles() {
    try {
        // Maak een GET-verzoek naar het list-files.cgi script
        const response = await fetch('/www2/cgi-bin/list-files.py');
        
        // Controleer of de response geldig is
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        // Zet de response om naar een JSON object
        const files = await response.json();

        // Als er bestanden zijn, toon ze
        if (files && files.length > 0) {
            document.getElementById('fileListSection').style.display = 'block';

            const fileList = document.getElementById('fileList');
            fileList.innerHTML = ''; // Verwijder oude bestandslijst

            // Voor elk bestand, voeg een lijst-item toe
            files.forEach(file => {
                const listItem = document.createElement('li');
                
                // Maak de bestandsnaam
                const fileNameSpan = document.createElement('span');
                fileNameSpan.textContent = file;
                fileNameSpan.className = 'file-name';

                // Maak de actie-knoppen (Download en Delete)
                const actionsDiv = document.createElement('div');
                actionsDiv.className = 'file-actions';

                // Download knop
                const downloadLink = document.createElement('a');
                downloadLink.href = `/www2/uploads/${file}`;
                downloadLink.className = 'action-button download-button';
                downloadLink.download = file;
                downloadLink.innerHTML = '<i class="fas fa-download"></i>';

                // Delete knop
                const deleteButton = document.createElement('button');
                deleteButton.className = 'action-button delete-button';
                deleteButton.innerHTML = '<i class="fas fa-trash"></i>';
                deleteButton.onclick = () => deleteFile(file); // Koppel de deleteFile functie

                // Voeg de knoppen toe aan de lijst
                actionsDiv.appendChild(downloadLink);
                actionsDiv.appendChild(deleteButton);

                // Voeg bestandsnaam en acties toe aan de lijst
                listItem.appendChild(fileNameSpan);
                listItem.appendChild(actionsDiv);

                // Voeg het lijst-item toe aan de bestandslijst
                fileList.appendChild(listItem);
            });
        } else {
            // Als er geen bestanden zijn, laat een bericht zien
            document.getElementById('fileListSection').style.display = 'none';
            console.log('No files found.');
        }
    } catch (error) {
        console.error('Error:', error);
    }
}

const fileInput = document.getElementById('fileInput');
const uploadButton = document.getElementById('uploadButton');
const uploadForm = document.getElementById('uploadForm');

// Enable button only if a file is selected
fileInput.addEventListener('change', () => {
    if (fileInput.files.length > 0) {
        uploadButton.disabled = false;
    } else {
        uploadButton.disabled = true;
    }
});

// Prevent form submission if no file is selected
uploadForm.addEventListener('submit', (event) => {
    if (fileInput.files.length === 0) {
        event.preventDefault(); // Prevent the form from being submitted
        alert('Please select a file before uploading.');
    }
});

// Aangepaste functie om een bestand te verwijderen
async function deleteFile(filename) {
    if (confirm(`Are you sure you want to delete ${filename}?`)) {
        try {
            // Maak een DELETE-verzoek naar het delete-file script
            const response = await fetch(`/www2/uploads/${filename}`, {
                method: 'DELETE'
            });

            // Controleer of het bestand succesvol werd verwijderd
            if (response.ok) {
                listFiles(); // Refresh de bestandslijst na verwijderen
            } else {
                alert('Failed to delete file');
            }
        } catch (error) {
            console.error('Error:', error);
            alert('Error deleting file');
        }
    }
}

// Roep listFiles() aan bij het laden van de pagina om bestanden op te halen
listFiles();
