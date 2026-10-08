import ast
import unittest
from pathlib import Path


STUBS = [
    Path(__file__).resolve().parents[1] / "python" / "pulsewaves" / "__init__.pyi",
    Path(__file__).resolve().parents[1]
    / "python"
    / "pulsewaves"
    / "pulsewaves_native.pyi",
]


class PythonStubDocsTest(unittest.TestCase):
    def test_stubs_parse(self):
        for stub in STUBS:
            with self.subTest(stub=stub.name):
                ast.parse(stub.read_text(), filename=str(stub))

    def test_public_functions_are_documented(self):
        tree = ast.parse(STUBS[1].read_text(), filename=str(STUBS[1]))
        undocumented = []

        for node in ast.walk(tree):
            if not isinstance(node, ast.FunctionDef):
                continue
            if node.name.startswith("__") and node.name.endswith("__"):
                continue
            if ast.get_docstring(node) is None:
                undocumented.append((node.lineno, node.name))

        self.assertEqual([], undocumented)


if __name__ == "__main__":
    unittest.main()
