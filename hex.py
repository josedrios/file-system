import subprocess

def run_hexdump():
    command = ['./Hexdump/hexdump.linuxM1', 'SampleVolume', '--start', '0', '--count', '25']
    
    try:
        # execute 
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        # decode the output
        output = result.stdout.decode('utf-8', errors='replace')
        
        print("Command Output:")
        print(output)
        
        if result.stderr:
            # error handling
            error_output = result.stderr.decode('utf-8', errors='replace')
            print("Command Error Output:")
            print(error_output)
    except subprocess.CalledProcessError as e:
        print(f"An error occurred: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

run_hexdump()