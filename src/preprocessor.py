import sys, os
import tempfile

def unread_character(f):
    f.seek(-1, os.SEEK_CUR) 

def read_character(f):
    return f.read(1)

def scan(f):
    c = f.read(1)
    if c == '/':
        c = f.read(1)
        if c == '/':
            while f.read(1) != '\n':
                pass
            return scan(f)
        elif c == '*':
            while not (f.read(1) == '*' and f.read(1) == '/'):
                pass
            return scan(f)
    else:
        if c == '':
            return None
        return c

if __name__ == '__main__':
    """ Input: A file in calculator language.
        Output: Name of a temporary file that is the input file with
        all comments removed."""
    if len(sys.argv) != 2:
        print "Usage: {0} file_name".format(sys.argv[0])
        exit(1)

    with open(sys.argv[1], 'r') as f:
        f_tmp = tempfile.NamedTemporaryFile(delete=False)

        while 1:
            scanned_char = scan(f)
            if not scanned_char:
                break
            f_tmp.write(scanned_char)
        print f_tmp.name
        f_tmp.close()
