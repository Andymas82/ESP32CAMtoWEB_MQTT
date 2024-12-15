<?php
// Download dir
$uploadDir = __DIR__ . '/uploads/';
$logOutput = "";

// Log
$logOutput .= "Start upload\n";
$logOutput .= "Request Method: " . $_SERVER['REQUEST_METHOD'] . "\n";

// Headers log
$logOutput .= "Request Headers:\n";
foreach ($_SERVER as $key => $value) {
    if (strpos($key, 'HTTP_') === 0) {
        $logOutput .= "$key: $value\n";
    }
}
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_FILES['file'])) {
    $upload_dir = 'uploads/';
    $file_name = $_FILES['file']['name'];
    $file_tmp = $_FILES['file']['tmp_name'];
    $file_size = $_FILES['file']['size'];
    $file_type = $_FILES['file']['type'];

    // Errors
    if ($file_size > 0 && move_uploaded_file($file_tmp, $upload_dir . $file_name)) {
        echo "File uploaded successfully!";
    } else {
        echo "Failed to upload file!";
    }
} else {
    echo "No file uploaded or incorrect request!";
}

// Create dir
if (!file_exists($uploadDir)) {
    if (mkdir($uploadDir, 0755, true)) {
        $logOutput .= "Created directory: $uploadDir\n";
    } else {
        $logOutput .= "Failed to create directory: $uploadDir\n";
    }
}

// POST-request
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $logOutput .= "Processing POST request\n";
    
    // Errors
    if (isset($_FILES['file'])) {
        $logOutput .= "File received\n";
        
        // uploading
        if ($_FILES['file']['error'] === UPLOAD_ERR_OK) {
            $fileTmpPath = $_FILES['file']['tmp_name'];
            $fileName = basename($_FILES['file']['name']);
            $destination = $uploadDir . $fileName;

            $logOutput .= "File temporary path: $fileTmpPath\n";
            $logOutput .= "File destination: $destination\n";
            
            if (move_uploaded_file($fileTmpPath, $destination)) {
                http_response_code(200);
                $logOutput .= "File uploaded successfully!";
                echo $logOutput;  // log
            } else {
                http_response_code(500);
                $logOutput .= "Failed to move uploaded file.";
                echo $logOutput;  // log
            }
        } else {
            http_response_code(400);
            $logOutput .= "File upload error: " . $_FILES['file']['error'];
            echo $logOutput;  // log
        }
    } else {
        http_response_code(400);
        $logOutput .= "No file uploaded.";
        echo $logOutput;  // log
    }
} else {
    http_response_code(405);
    $logOutput .= "Invalid request method. Only POST is allowed.";
    echo $logOutput;  // log
}
?>
