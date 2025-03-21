import os
import subprocess
import json


def clone_or_update_repo(repo_url, path, branch):
    command = []
    command.append('git')
    command.append('clone')
    command.append('--depth=1')

    if branch:
        command.append('-b')
        command.append(branch)

    command.append(repo_url)
    command.append(path)

    subprocess.run(command)


def modify_m5gfx_cmake_file():
    print("try modify m5gfx cmake file..")
    cmake_path = './dependencies/M5GFX/CMakeLists.txt'

    if os.path.exists(cmake_path):
        with open(cmake_path, 'r') as f:
            lines = f.readlines()

        # Search for the line that contains the comment and uncomment it
        for i, line in enumerate(lines):
            if "# list(APPEND COMPONENT_REQUIRES arduino-esp32)" in line:
                lines[i] = "list(APPEND COMPONENT_REQUIRES arduino-esp32)\n"
                break

        # Write the changes back to the file
        with open(cmake_path, 'w') as f:
            f.writelines(lines)

        print("ok")
    else:
        print(f"Error: {cmake_path} does not exist")


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    config_path = os.path.join(script_dir, 'repos.json')

    with open(config_path) as f:
        repos = json.load(f)

    for repo in repos:
        repo_path = os.path.join(script_dir, repo['path'])
        branch = repo.get('branch')
        clone_or_update_repo(repo['url'], repo_path, branch)

    modify_m5gfx_cmake_file()


if __name__ == "__main__":
    print("start fetching dependent repos..")
    main()
