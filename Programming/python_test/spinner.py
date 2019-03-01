import sys

class Spinner:

    # Constructor

    def __init__(self):

        self.symbols = [' |', ' /', ' -', ' \\', ' |', ' /', ' -', 'END']
        self.curSymbol = 0

        sys.stdout.write("\b\b\b%s " % self.symbols[self.curSymbol])
        #sys.stdout.write("%s " % self.symbols[self.curSymbol])
        sys.stdout.flush()

    def Spin(self):

        if self.symbols[self.curSymbol] == 'END':
            self.curSymbol = 0

        sys.stdout.write("\b\b\b%s " % self.symbols[self.curSymbol])
        #sys.stdout.write("%s " % self.symbols[self.curSymbol])
        sys.stdout.flush()
        self.curSymbol += 1

# End Spinner Class

