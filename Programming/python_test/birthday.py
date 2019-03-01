birthdays = { 'Alice': 'Apr 1', 'Bob': 'Dec 2', 'Carol': 'Mar 4'}

while True:
    print('Enter a name: (blank to quit)')
    name = raw_input()
    if name == '':
        break

    if name in birthdays:
        print(birthdays[name] + ' is the birthday of' + name)
    else:
        print('I do not have birthday information for ' + name)
        print('What is their birthday?')
        bday = raw_input()
        birthdays[name] = bday
        print('Birthday database updated.')



