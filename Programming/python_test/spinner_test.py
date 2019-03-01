import signal
import spinner
from time import sleep




# Create timeout class to handle capture duration

class myTimeout(Exception):
  pass

# Create a signal handler that raises a timeout event
# when the capture duration is reached

def handler(signum, frame):
  if True:
    print 'Capture Complete', signum
    print

    raise myTimeout()



if __name__ == '__main__':
  capturedDuration = 1 * 5 # seconds

  signal.signal(signal.SIGALRM, handler)
  signal.alarm(capturedDuration)

  sp = spinner.Spinner()

  try:
    while True:
      sp.Spin()
      sleep(0.2)

  except myTimeout:
    pass





