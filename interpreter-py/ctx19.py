class Contextual19Parser:

    def __init__(self, filepath: str):
        """Init the class and read the file.

        Args:
            filepath (str): Path to .ctx19 file to read.

        Raises:
            FileNotFoundError: The file is not exists.
            PermissionError: You're not allowed to access to this file. This
                error also can occur when the path you specified is directory,
                not a file.

        """

        self.file = open(filepath, mode="r", encoding="utf-8")

        self.parseFile()

    def parseFile(self):
        """Read self.file and convert it to object. It will be the same as the
        JSON-implementation of Contextual19.
        """

        def expectTabs(lines, number):
            """Cut first {number} chars if those are tabs, raise TypeError
            otherwise.
            """
            # global self.cursor

            if lines[self.cursor][:number] != "\t" * number:
                raise TypeError(
                    f"Incorrect indentation. Expected tabs: {number}"
                )
            else:
                return lines[self.cursor][number:]

        def moveTo(lines, text):
            """Moves reading self.cursor to next (or first) text
            """
            # global self.cursor

            while lines[self.cursor] != text:
                self.cursor += 1

            # Data will begin on the next line
            self.cursor += 1

        def catchSelector(lines):
            """Catch selector and return its name and position.
            """
            # global self.cursor

            # Literal positions of selectors
            words = {
                "first": 1, "second": 2, "third": 3, "fourth": 4, "fifth": 5
            }

            data = expectTabs(lines, 1).split(" ")

            position = 1
            # There's only one word as selector
            if len(data) == 1:
                self.cursor += 1
                return (position, data[0])

            name = data[1]

            if data[0] in words:
                # Look for literal position in dictionary
                position = words[data[0]]
            else:
                # Cut out 'th' from the and
                position = int(data[0][:-2])

            self.cursor += 1
            return (position, name)

        def catchComparison(lines):
            """Catch comparisons, returns name, value and type of comparison.
            'name is value' -> ('name', 'value', True)
            'name is not value' -> ('name', 'value', False)
            """
            # global self.cursor

            data = expectTabs(lines, 2).split(" ")

            name = data[0]

            compIsPositive = (
                False
                if (data[1] == "is" and data[2] == "not")
                else True
            )

            value = data[2] if compIsPositive else data[3]

            self.cursor += 1
            return (name, value, compIsPositive)

        def catchAssignment(lines):
            """Catch assignment, returns name and assigning value.
            """
            # global self.cursor

            data = expectTabs(lines, 1).split(" ")

            self.cursor += 1
            return (data[0], data[2])

        def collectComparisons(lines):
            """Collects all the comparisons for selector, write them to
            dict and return it.
            """
            # global self.cursor
            selector = dict()
            # This will collect all comparisons when they're still
            # appearing
            try:
                while True:
                    name, value, isPositive = catchComparison(
                        lines
                    )
                    selector[name] = [isPositive, value]
            except TypeError:
                return selector

        def collectAssignings(lines):
            """Collects all the assignings, write them to dict and return it.
            """
            assignings = dict()
            try:
                while True:
                    name, value = catchAssignment(lines)
                    assignings[name] = value
            except TypeError:
                return assignings

        def collectSelectors(lines):
            """Collects all the selectors for rule, write them to dict and
            return it
            """
            conditions = dict()
            try:
                while True:
                    position, name = catchSelector(lines)
                    conditions[name] = collectComparisons(lines)
                    conditions[name]["position"] = position
            except TypeError:
                return conditions

        def collectRules(lines):
            """Collects rules and returns it like list of dicts.
            """
            rules = list()
            try:
                while True:
                    rule = dict()
                    moveTo(lines, "if")
                    rule["if"] = collectSelectors(lines)
                    moveTo(lines, "then")
                    rule["then"] = collectAssignings(lines)
                    rules.append(rule)
            except (TypeError, IndexError):
                return rules

        # Remember reading cursor
        self.cursor = 0

        # Remember parsed data
        self.data = list()

        lines = [line.rstrip('\n') for line in self.file]
        self.cursor = 0

        # This will iterate reading to the end of file
        self.data = collectRules(lines)

        # Delete cursor as not needed long
        del self.cursor
