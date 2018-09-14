""" 
Warning: This program currently has access vulnerabilities (access folders via cmd-line args).
         Do not use in the wild.

Known issues:
         - Determines directory that contains student's edited files by traversing
           through their submitted folder to find the desired files
           If more than one file with the same name exists in any descendant folder,
           that file will be copied into the tmp/ folder and graded.

           If a student is getting a low grade from batch_grader but a better grade from autograder,
           make sure they don't have copies of the files of interest.

         Author: Andrew Dudley
         Purpose: Automatically grade AI assignments and generate .csv files
         with uid, grade, and late days used
    
"""
import zipfile
import argparse
import datetime
import os, glob, sys, re
sys.dont_write_bytecode = True

curr_dir = os.getcwd()
ORIGINAL_SUBMISSIONS = "original_submissions/"
SUBMISSIONS = "submissions/"

def unzip_original_submissions(args):
    student_folders = []
    for file in glob.glob(ORIGINAL_SUBMISSIONS + '*'+(args.userid if args.userid else '') + "*.zip"):
        file_name = os.path.splitext(os.path.basename(file))[0]
        rel_path = SUBMISSIONS + file_name
        zip_file = zipfile.ZipFile(file, 'r')
        zip_file.extractall(rel_path)
        student_folders.append(rel_path)
    return student_folders
        
def get_submissions(args):
    """ 
    If -x command used, this function retrieves list of folders directly from 'submissions'
    folder instead of from the unzip_original_submissions function
    """
    student_folders = []
    for folder in glob.glob(SUBMISSIONS + '*'+(args.userid if args.userid else '') + '*'):
        student_folders.append(folder)
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


    if not os.path.exists("tmp/"+'dataClassifier.py'):
        shutil.copy(os.path.join('classification', 'dataClassifier.py'), 'tmp/' + 'dataClassifier.py')
    if not os.path.exists("tmp/models.py"):
        shutil.copy(os.path.join('classification', 'models.py'), 'tmp/' + 'models.py')
    if not os.path.exists("tmp/tensorflow_util.py"):
        shutil.copy(os.path.join('classification', 'tensorflow_util.py'), 'tmp/' + 'tensorflow_util.py')
    if not os.path.exists("tmp/perceptron.py"):
        shutil.copy(os.path.join('classification', 'perceptron.py'), 'tmp/' + 'perceptron.py')
            
def runAutograder():
    import subprocess, re, time
    os.chdir("tmp")
    start = time.time()
    proc = subprocess.Popen(['python2', 'autograder.py', '--mute', 'True'], stdout=subprocess.PIPE)
    stdout = proc.stdout.read()
    
    end = time.time()
    grades = re.search("Provisional grades(.*)\nYour", stdout, flags=re.DOTALL)
    result = None
    if grades:
        result = grades.group(1)
    os.chdir("../")
    return result, end - start, stdout
    
def main(args):
    # Get current time for use in logs
    current_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M")

    # Unzip .zip files (TODO: support .rar?)
    if not args.dontextract:
        student_folders = unzip_original_submissions(args)
    else:
        student_folders = get_submissions(args)
    # Sort student folders so they're evaluated in the order of their username
    student_folders.sort()

    if not os.path.exists("logs"):
        os.mkdir("logs")
    if not os.path.exists("logs/"+current_time):
        os.mkdir("logs/"+current_time)

    with open('results.csv', 'w+') as f:
        """ 
        For each student submission, copy project files into the tmp/ folder, 
        run the  autograder, and save the results.     
        """

        for student_folder in student_folders:
            tmp = student_folder.split('_')
            sid = tmp[1]
            submission_datetime = tmp[3] 
            dt_submission = datetime.datetime.strptime(submission_datetime, "%Y-%m-%d-%H-%M-%S")
            dt_due = datetime.datetime(2017, 11, 20, 23, 59, 59)
            dt_diff = dt_due - dt_submission
            if dt_diff.days < 0:
                late_days = -1*dt_diff.days
            else:
                late_days = 0
            moveFilesToTestFolder(student_folder, ["perceptron.py", "answers.py", "solvers.py", "search_hyperparams.py", "features.py"])
            grades, time, stdout = runAutograder()
            with open(os.path.join('logs', current_time, sid+'.txt'), 'w+') as sf:
                sf.write(stdout) 

            total_grade = None
            if grades:
                question_grades = re.findall(r"Question q\d+[a-z]*: (.*)/(\d+)", grades)
                total_tuple = float( re.search("Total: (\d+)/(\d+)", grades).group(1) )
                grade = sum([float(i) for i,j in question_grades[3:]])
                f.write("{}, {}, {}\n".format(sid, grade, late_days))
                print("{}, {}, {}".format(sid, grade, late_days))
            else:
                f.write("{}, {}, {}\n".format(sid, "ERROR", late_days))
                print("{}, {}, {}\n".format(sid, "ERROR", late_days))
            f.flush()
            os.fsync(f.fileno())
 
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", "--userid", help="asurite id")
    parser.add_argument("-x", "--dontextract", action="store_true", help="skip extraction from original_submissions (uses folders in 'Submissions' directory)")
    args = parser.parse_args()
    main(args)
