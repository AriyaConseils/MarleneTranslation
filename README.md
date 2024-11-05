# marleneTranslation

**marleneTranslation** is a translation management tool designed for Qt `.ts` files. It streamlines the workflow for creating, updating, and managing translations across multiple languages and integrates smoothly into CI/CD pipelines. This tool provides both a GUI for easy project management and a powerful CLI for automation, making it adaptable for complex translation projects.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Command Line Usage](#command-line-usage)
- [Examples](#examples)
- [Future Updates](#future-updates)

---

## Features

- **Custom Build System for Translation Files**  
  Generate binary `.qm` files from `.ts` files with dynamic argument support in build commands (e.g., `${sourceName}`, `${sourceDir}`, `${sourcePath}`), allowing flexibility in file paths and compiler choice.

- **Project-Based CLI Commands**  
  CLI mode supports commands like `--i18n` and `--l10n` for generating i18n files and compiling localization files, enabling users to automate translation updates for existing projects.

- **Integration-Ready for CI/CD**  
  Built with automation in mind, marleneTranslation can be integrated into CI/CD pipelines, enabling continuous translation updates and localization builds without manual intervention.

- **Flexible Translation Options**  
  Support for `--translate all` and `--fullUpdate` options for comprehensive re-translations, making it easy to refresh translations across all project files.

---

## Installation

1. **Download the latest release** from the [Releases](https://github.com/AriyaConseils/marleneTranslation/releases) page.
2. Run the installer `MarleneTranslationInstaller.exe` and follow the setup instructions.

Alternatively, you can clone this repository and build the project manually:

```bash
git clone https://github.com/AriyaConseils/marleneTranslation.git
cd marleneTranslation
```
# Follow the specific build instructions (if needed)

## Getting Started

### Creating a Project
Before using marleneTranslation in CLI mode, create a project in the GUI:

1. Launch the application.
2. Navigate to **File > New Project** and specify your project settings.
3. Save your project as a `.rrt` file.

Once your project is created, you can use the CLI commands for automated translation and localization management.

---

## Command Line Usage
marleneTranslation provides a robust CLI with options for managing translations and localization tasks.

### CLI Options
- `--source <path>`: Specifies the source project file (.rrt).
- `--i18n`: Generates i18n files for the project.
- `--translate [all]`: Translates entries in `.ts` files. Use `all` to translate all entries, regardless of existing translations.
- `--fullUpdate`: Performs a full re-evaluation of all translations, including those previously translated.
- `--l10n`: Compiles `.ts` files to binary `.qm` files for localization.
- `--ts-file <path>`: Specifies the path to a specific `.ts` file for targeted translation.

> **Note**: Ensure your project is created in the GUI before using CLI commands.

---

## Examples

### 1. Generate i18n Files for a Project
   ```bash
   MarleneTranslation --source C:/path/to/projectFile.rrt --i18n
```
**Description**: Scans the project directory specified in `projectFile.rrt` and generates the internationalization files needed to initialize the translation process.

---

2. **Full Translation of All Entries**
 ```bash
   MarleneTranslation --source C:/path/to/projectFile.rrt --translate all --fullUpdate
 ```
   **Description**: Performs a complete re-translation of every entry in all `.ts` files in the project, ensuring every text is updated. This is especially useful after major content changes that require all translations to be refreshed.

---

3. **Full CI/CD Workflow**
 ```bash
   MarleneTranslation --source C:/path/to/projectFile.rrt --i18n --translate all --fullUpdate --l10n
 ```
   **Description**: Automates the entire translation and localization workflow, creating i18n files, re-translating entries, and compiling `.qm` files for production. Ideal for CI/CD pipelines, allowing marleneTranslation to manage and update translations in an automated manner.

---

## Future Updates
- **CLI Project Creation**: Future versions will support creating new projects directly from the CLI.
- **Enhanced Language Support**: Improved handling of region-specific translations.
- **Customizable Output**: Ability to customize the output structure for generated files.

---

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## Contributing
We welcome contributions! Please read the [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on how to contribute.

For any questions or issues, feel free to open an issue on GitHub or reach out to us.

**marleneTranslation** simplifies the translation workflow, whether you’re working on a single language or managing large, multi-language projects. With both GUI and CLI modes, it’s designed to fit into any workflow, making translation and localization easier than ever.

