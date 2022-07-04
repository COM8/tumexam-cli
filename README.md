# TUMexam Client
C++ client for https://tumexam.de/ to manage students and exams.

## Examples

![Dark Correction Overview](images/correction_dark.png)

![Dark Submission Overview](images/submission_dark.png)

![Dark Students Overview](images/students_dark.png)

![Dark Feedback Overview](images/feedback_dark.png)

## [<img src="https://flathub.org/assets/badges/flathub-badge-en.svg" height="50">](https://flathub.org/apps/details/de.tumexam.cli)
The TUMexam client can be build and installed using Flatpak.

The build instructions can be found here: https://github.com/flathub/de.tumexam.cli

## Building
### Requirements

#### Fedora
```
sudo dnf install gtkmm4.0-devel libcurl-devel gcc cmake git
```

#### Debian/Ubuntu
```
sudo apt install libgtkmm-4.0-dev libcurl4-openssl-dev gcc cmake git
```

### Compiling
```
git clone https://github.com/COM8/tumexam-cli.git
cd tumexam-cli
mkdir build
cd build
cmake ..
cmake --build .
```

### Installing
```
sudo cmake --build . --target install
```
