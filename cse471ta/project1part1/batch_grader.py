import zipfile
import os, glob, sys, re
sys.dont_write_bytecode = True

curr_dir = os.getcwd()
ORIGINAL_SUBMISSIONS = "original_submissions/"
SUBMISSIONS = "submissions/"

def unzip_original_submissions():
    student_folders = []
    for file in glob.glob(ORIGINAL_SUBMISSIONS + "*.zip"):
        file_name = os.path.splitext(os.path.basename(file))[0]
        rel_path = SUBMISSIONS + file_name
        zip_file = zipfile.ZipFile(file, 'r')
        zip_file.extractall(rel_path)
        student_folders.append(rel_path)
    return student_folders
        
def moveFilesToTestFolder(student_folder, test_files):
    import shutil
    # Delete tmp folder
    if os.path.exists("tmp"):
        for file in test_files:
            if os.path.exists("tmp/"+file):
                os.remove("tmp/"+file)
            if os.path.exists("tmp/"+file+'c'):
                os.remove("tmp/"+file+'c')
                        
    # Copy student files
    for dirpath, dirnames, filenames in os.walk(student_folder):
        for filename in [f for f in filenames if f in test_files]:
            shutil.copy(os.path.join(dirpath, filename), "tmp/" + filename)


    if not os.path.exists("tmp/"+'searchAgents.py'):
        shutil.copy(os.path.join('search', 'searchAgents.py'), 'tmp/' + 'searchAgents.py')

            
def runAutograder():
    import subprocess, re
    os.chdir("tmp")
    proc = subprocess.Popen(['python2', 'autograder.py', '--mute', 'True'], stdout=subprocess.PIPE)
    tmp = proc.stdout.read()
    grades = re.search("Provisional grades(.*)\nYour", tmp, flags=re.DOTALL)
    result = None
    if grades:
        result = grades.group(1)
    os.chdir("../")
    return result
    
def main():
    student_folders = unzip_original_submissions()
    student_folders.sort()
    for student_folder in student_folders:
        tmp = student_folder.split('_')
        sid = tmp[1]
        attempt = tmp[3]
        print sid, attempt,
        moveFilesToTestFolder(student_folder, ["search.py", "searchAgents.py"])
        grades = runAutograder()
        total_grade = None
        if grades:
            total_grade = re.search("Total: (.*)", grades).group(1)
        print grades
 
if __name__ == '__main__':
    main()
