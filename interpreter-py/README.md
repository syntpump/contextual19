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

### Keywords
* `bool` `saveContext` (default is `True`)  
	This parameter set transformation resistance of the rules. Suppose, we have this sentence:  
	`[{a:0}, {a:1}, {a:2}]`,  
	and this rules (inline-written):  
	#1: `if previous a is 0   then a becomes 2`,    
	#2: `if previous a is 1   then a becomes 3`.  
	After executing rule #1 sentence becomes:  
	`[{a:0}, {a:2}, {a:2}]`  
	Now the rule #2 is not appliable in this contexts. That is what means "transformation nonresistance". To make the rule #2 work, `saveContext` must be set to `True` and Ctx19 module will remember original sentence before applying rules (via `deepcopy`, so some additional memory will be needed).

### Methods

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
	Missing properties will be skipped.
* `save(filepath: str) -> void`  
	This method will simply save the rules stored in `self.data` into file you passed as `filepath`.

### Private methods

There's also some methods that should not be used in your code, but they are widely used in `Contextual19Parser.apply` method and they can be useful in some cases.

* `ruleIsAppliable(rule: dict, sentence: list, token: int) -> bool`  
	Check if the given `rule` is appliable to the `token`th token in the `sentence`. It'll check the context.

## `ctx19.parsers.Contextual19FileParser`

This is the children of `Contextual19Parser` class with the changed initialization method.

### Initialization

To initialize this parser just pass path to file with rules as the `f` parameter:

```python
from ctx19.parsers import Contextual19FileParser
parser = Contextual19FileParser(open("path/to/file.ctx19"))
```

### Keywords
* `bool` `astext`  
	Set this to `True` to read text from `f` parameter instead of dealing with it as file.
	**Example of use:**
	```python
	data = '''
	if
		...
	then
		...
	...
	'''
	parser = Contextual19FileParser(data, astext=True)
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
