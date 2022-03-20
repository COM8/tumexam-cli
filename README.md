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

## Flatpak
The TUMexam client can be build and installed using Flatpak.

### Requirements
#### Fedora
```
sudo dnf install flatpak
flatpak install flathub org.gnome.Sdk//41 org.gnome.Platform//41
```

#### Debian/Ubuntu
```
sudo dnf install flatpak
flatpak install flathub org.gnome.Sdk//41 org.gnome.Platform//41
```

### Building
```
git clone https://github.com/COM8/tumexam-cli.git
cd tumexam-cli
flatpak-builder --force-clean flatpak_build_dir de.tumexam.cli.yml
```

### Installing
```
flatpak-builder --user --install --force-clean flatpak_build_dir de.tumexam.cli.yml
```

### Uninstalling
```
flatpak uninstall de.tumexam.cli
```

## Executing
```
tumexam-cli
```
