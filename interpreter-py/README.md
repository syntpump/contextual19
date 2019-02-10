[![Pypi](https://img.shields.io/pypi/v/ctx19.svg)](https://pypi.python.org/pypi/ctx19)

# Contextual19 Python Interpreter

This module provides you 2 classes to work with Ctx19.  
You can [read here about Contextual19](https://github.com/syntpump/contextual19/).

## `ctx19.parsers.Contextual19Parser`

This is just regular parser that takes rules in object representation.

### Initialization

To initialize this class, just call it and take there rules in object representation:

```python
from ctx19.parsers import Contextual19Parser
data = [
	{
		"if": ...,
		"then": ...
	},
	...
]
parser = Contextual19Parser(data)
```

Data you passed will be stored in `parser.data`.

### methods

* `apply(sentence: list) -> list`  
	This method apply rules, stored in `self.data` to the sentence you passed.  
	`sentence` is the list of dictionaries which represents tokens with their properties. It can look like this:
	```json
	[
		{
			"tense": "past",
			"gender": "neutral",
			"some_other_stuff": "other_value"
		},
		{
			"tense": "future",
			"voice": "active",
			"another_property": "another_value"
		}
	]
	```
	If some property will be missing while rule applying then it will be skipped.
* `save(filepath: str) -> void`  
	This method will simply save the rules stored in `self.data` into file you passed as `filepath`.

* `applyToSentence(rule: dict, sentence: list) -> list`  
	This method will apply the given rule to every token in the sentence if possible.

### Private methods

There's also some methods that should not be used in your code, but they are widely used in `Contextual19Parser.apply` method and they can be useful in some cases.

* `applyRule(rule: dict, token: dict) -> dict`  
	Apply assignments from some rule to the given token. This won't check if the given rule is really appliable to the token in this context. Resulting token will be returned.
* `ruleIsAppliable(rule: dict, sentence: list, token: int) -> bool`  
	Check if the given `rule` is appliable to the `token`th token in the `sentence`. It'll check the context.
* `applyIfPossible(rule: dict, sentence: list, token: int) -> list`  
	It will apply some rule to the token if it's appliable in this context. Resulting sentence will be returned.
* `applyRulesTo(rules: list, sentence: list, token: int) -> list`  
	Apply each rule in the given list to the `token`-th token in the `sentence` if they're appliable.

## `ctx19.parsers.Contextual19FileParser`

This is the children of `Contextual19Parser` class with the changed initialization method.

### Initialization

To initialize this parser just pass path to file with rules as the `filepath` parameter:

```python
from ctx19.parsers import Contextual19FileParser
parser = Contextual19FileParser("path/to/file.ctx19")
```

Since this is the `Contextual19Parser` children, it'll inherit all the parent methods, but one new private was added.

### Private methods

* `parseFile(void) -> void`  
	This will parse Contextual19 syntax from the file saved in `self.file`. You can change the file and call this method again.

## Reading other object notation formats

From 1.2 version JSON and YAML parsers was moved in order to make ctx19 independent from other packages, but you can still do it yourself. Here's the example:
```python
import json
from ctx19.parsers import Contextual19Parser
ctx = Contextual19Parser(json.load("path/to/file.json"))
```
