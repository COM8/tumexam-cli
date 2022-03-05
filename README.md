# TUMexam Client
C++ client for https://tumexam.de/ to manage students and exams.

## Examples

![Dark Submission Overview](images/dark_submissions.png)
![Light Submission Overview](images/light_submissions.png)

![Dark Submission Notes](images/dark_correction_notes.png)
![Light Submission Notes](images/light_correction_notes.png)

![Dark Correction Overview](images/dark_correction.png)
![Light Correction Overview](images/light_correction.png)

## Requirements

### Fedora
```
sudo dnf install gtkmm40-devel libcurl-devel gcc cmake git
```

### Debian/Ubuntu
```
sudo apt install libgtkmm-4.0-dev libcurl4-openssl-dev gcc cmake git
```

## Building
```
git clone https://github.com/COM8/tumexam-cli.git
cd tumexam-cli
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing
```
tumexam-cli
```
