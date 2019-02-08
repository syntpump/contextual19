class Contextual19Parser:
    """This is Contextual19 parser. It takes the rules in object representation
    and can process your data using them.
    """

    def _init__(self, data: dict):
        """Init the class and remember the rules.

        Args:
            data (dict): Rules in object representation.

        """

        self.data = data

    def applyRule(self, rule: dict, token: dict) -> dict:
        """Apply assignments from "then" block of rule to the token.

        Args:
            rule (dict): Rule dict from self.data which contains 'if' and
                'then' properties.
            token (dict): Token dictionary which properties will be changed.
                Example:
                token = {
                    "a": 0, "b": 1
                }
                Under this rules:
                    a becomes 1
                    b becomes 2
                `token` becomes: {
                    "a": 1,
                    "b": 2
                }

        Returns:
            dict: Resulting token.

        """

        for key in rule["then"]:
            if key in token:
                token[key] = rule["then"][key]

        return token

    def ruleIsAppliable(self, rule: dict, sentence: list, token: int) -> bool:
        """Check if rule as appliable to some token in the sentence.

        Args:
            rule (dict): Rule dict from self.data which contains 'if' and
                'then' properties.
            sentence (list of dict): List of dicts of tokens.
                Tokens should look like:
                {"voice": ..., "tense": ..., other properties...}
            token (int): Number of token (starting from 0) which propriety will
                be checked.

        Returns:
            bool: True, if rule is appliable to this token, False otherwise.

        """

        def getRelativePosition(selector):
            """Returns number of relative position (less than 0 for left
            context and bigger than 0 otherwise) for the given selector.
            Do not process absolute positions as 'end' and 'beginning'.
            Example:
                second next -> 2
                4th previous -> -4
                token -> 0
                end, beginning -> 0
            """

            if selector["name"] in ["end", "beginning", "token"]:
                return 0

            if selector["name"] == "previous":
                return -selector["position"]
            else:
                return selector["position"]

        def getAbsolutePosition(selector, token: int, sentence):
            """Returns number of absolute position of token to which the given
            selector links.
            Example:
                Suppose len(sentence) = 10
                selector = end, token = 5 -> 10
                selector = beginning, token = 6 -> 0
                selector = second next, token = 2 -> 4
            """

            if selector["name"] == "beginning":
                return 0
            elif selector["name"] == "end":
                return len(sentence) - 1
            else:
                position = token + getRelativePosition(selector)
                # Return None if position is out of sentence range.
                if position < 0 or position >= len(sentence):
                    return None
                else:
                    return position

        def checkComparisons(position, selector, sentence):
            """Check if some token (given as 'position') meets the selector
            requirements.
            """

            for key in selector:
                # Skip the service properties
                if key in ["name", "position"]:
                    continue
                # If at least one key of token is not present the the whole
                # rule is not appliable
                if key not in sentence[position]:
                    return False
                # Check other ones
                isPositive = selector[key][0]
                if(
                    (
                        sentence[position][key] == selector[key][1] and
                        not isPositive
                    ) or (
                        sentence[position][key] != selector[key][1] and
                        isPositive
                    )
                ):
                    return False

            return True

        for selector in rule["if"]:
            position = getAbsolutePosition(selector, token, sentence)
            # If at least one of selectors is out of range then the whole rule
            # is not appliable.
            if position is None:
                return False
            else:
                if not checkComparisons(position, selector, sentence):
                    return False

        # If there was no fails before, this code will execute
        return True

    def applyIfPossible(self, rule: dict, sentence: list, token: int) -> list:
        """Check if rule is appliable to the specific token and apply it then.

        Args:
            rule (dict): Rule dict from self.data.
            sentence (list of dict): List of dicts of tokens.
                Tokens should look like:
                {"voice": ..., "tense": ..., other properties...}
            token (int): Number of token to check.

        Return:
            list of dict: Resulting sentence

        """

        if self.ruleIsAppliable(rule, sentence, token):
            sentence[token] = self.applyRule(rule, sentence[token])

        return sentence

    def applyRulesTo(self, rules: list, sentence: list, token: int) -> list:
        """Check each rule if it is appliable to the given token and apply it
        then.

        Args:
            rules (list of dicts): Rules from self.data with standard 'if' and
                'then' properties.
            sentence (list of dict): List of dicts of tokens.
                Tokens should look like:
                {"voice": ..., "tense": ..., other properties...}
            token (int): Number of token to apply.

        Returns:
            list of dict: Resulting sentence

        """

        for rule in rules:
            sentence = self.applyIfPossible(rule, sentence, token)

        return sentence

    def apply(self, sentence: list) -> list:
        """Apply rules to tokens of the sentence.

        Args:
            sentence (list of dict): List of dicts of tokens.
                Tokens should look like:
                {"voice": ..., "tense": ..., other properties...}

        """

        for token in range(len(sentence)):
            sentence = self.applyRulesTo(self.data, sentence, token)

        return sentence


class Contextual19FileParser(Contextual19Parser):
    """This class will parse data from file to use.
    """

    def __init__(self, filepath: str):
        """Init the class and read the file.

        Args:
            filepath (str): Path to .ctx19 file to read.

        Raises:
            FileNotFoundError: The file does not exist.
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

        def collectAssignments(lines):
            """Collects all the assignments, write them to dict and return it.
            """
            assignments = dict()
            try:
                while True:
                    name, value = catchAssignment(lines)
                    assignments[name] = value
            except (TypeError, IndexError):
                return assignments

        def collectSelectors(lines):
            """Collects all the selectors for rule, write them to dict and
            return it
            """
            conditions = list()
            try:
                while True:
                    selector = dict()
                    position, name = catchSelector(lines)
                    selector["position"] = position
                    selector["name"] = name
                    selector.update(collectComparisons(lines))
                    conditions.append(selector)
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
                    rule["then"] = collectAssignments(lines)
                    rules.append(rule)
            except (TypeError, IndexError):
                return rules

        # Remember reading cursor
        self.cursor = 0

        lines = [line.rstrip('\n') for line in self.file]
        self.cursor = 0

        # This will iterate reading to the end of file
        self.data = collectRules(lines)

        # Delete cursor as not needed long
        del self.cursor
