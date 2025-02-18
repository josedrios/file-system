import subprocess

def run_commands():
    try:
        # Run the 'make' command
        print("Running 'make'...")
        make_result = subprocess.run(['make'], check=True)
        print("'make' executed successfully.")

        # Run the 'make run' command
        print("Running 'make run'...")
        make_run_result = subprocess.run(['make', 'run'], check=True)
        print("'make run' executed successfully.")

        # Run the 'make run' command
        print("Running 'make run'...")
        make_clean_result = subprocess.run(['make', 'clean'], check=True)
        print("'make clean' executed successfully.")

    except subprocess.CalledProcessError as e:
        print(f"An error occurred while executing: {e.cmd}")
        print(f"Exit status: {e.returncode}")

if __name__ == '__main__':
    run_commands()
