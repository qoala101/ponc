import os


def GetFilesWithExtension(dir, ext, full_path):
    headers = []

    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith("." + ext):
                if full_path:
                    headers.append(os.path.join(root, file))
                else:
                    headers.append(file)

    return headers


third_party_headers = GetFilesWithExtension("./build/_deps", "h", False)
project_files = GetFilesWithExtension(
    "./include", "h", True) + GetFilesWithExtension("./src", "cc", True)

print("Replacing third-party headers...")

for file in project_files:
    stream = open(file, "rt")
    data = stream.read()

    for header in third_party_headers:
        data = data.replace("\"" + header + "\"", "<" + header + ">")

    stream.close()

    stream = open(file, "wt")
    stream.write(data)
    stream.close()

print("Done")
