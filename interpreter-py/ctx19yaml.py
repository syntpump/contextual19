"""This parser works just like Contextual19FileParser, but it loads data from
YAML file instead of parsing Ctx19 syntax.
"""

from .ctx19 import Contextual19FileParser
import yaml


class Contextual19YAMLFileParser(Contextual19FileParser):

    def parseFile(self):
        """Read self.file and parse it like YAML.
        """

        self.data = yaml.load(self.file)
