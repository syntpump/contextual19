"""This parser works just like Contextual19FileParser, but it loads data from
JSON file instead of parsing Ctx19 syntax.
"""

from .ctx19 import Contextual19FileParser
import json


class Contextual19JSONFileParser(Contextual19FileParser):

    def parseFile(self):
        """Read self.file and parse it like JSON.
        """

        self.data = json.load(self.file)
