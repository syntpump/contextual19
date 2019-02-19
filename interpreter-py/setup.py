import setuptools

with open("README.md", encoding="utf-8") as fp:
    long_description = fp.read()

setuptools.setup(
    name="ctx19",
    version="1.3",
    description="A Python driver for Contextual19 markup language",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/syntpump/contextual19",
    author="Syntpump",
    author_email="lynnporu@gmail.com",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License"
    ],
    keywords="nlp",
    packages=setuptools.find_packages(),
    python_requires=">3",
    project_urls={
        "Syntpump on GitHub": "https://github.com/syntpump"
    }
)
