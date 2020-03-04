def print_line():
    print("-" * 60)

def print_full_header(build_step_name):
    print_line()
    print(" Build Step /// {}".format(build_step_name))

def print_footer():
    print_line()

def log(level, data):
    print("{0}: {1}".format(level, data))
