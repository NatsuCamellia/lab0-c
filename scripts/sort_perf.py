import subprocess
import time
import re

def run_test(unit: int, iters: int):
    regex = re.compile(r'\d+')
    p = subprocess.Popen(['./qtest', '-v', '0'], text=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    time.sleep(1)

    for i in range(1, iters + 1):
        size = unit * i

        # init queue
        p.stdin.write(f"new\nih RAND {size}\n")
        p.stdin.flush()

        # sort
        start_time = time.perf_counter()
        p.stdin.write(f"sort\n")
        p.stdin.flush()
        out = p.stdout.readline()
        end_time = time.perf_counter()

        # record time
        cmpcnt = regex.findall(out)[0]
        wait_time = end_time - start_time
        print(f'{size:7d} {cmpcnt:8s} {wait_time:.4f}', flush=True)

        # free
        p.stdin.write("free\n")
        p.stdin.flush()

subprocess.run(['make', 'clean'], stdout=subprocess.DEVNULL)
subprocess.run(['make', 'CMPCNT=1'], stdout=subprocess.DEVNULL)
print("Default sort")
run_test(unit=10000, iters=100)

subprocess.run(['make', 'clean'], stdout=subprocess.DEVNULL)
subprocess.run(['make', 'CMPCNT=1', 'LINUX=1'], stdout=subprocess.DEVNULL)
print("Linux list_sort")
run_test(unit=10000, iters=100)