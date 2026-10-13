import importlib.util
from pathlib import Path
import unittest

DM = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('shell_stubs', DM / 'ios/tools/shell_stubs.py')
stubs = importlib.util.module_from_spec(spec)
spec.loader.exec_module(stubs)


class BrightnessDataTest(unittest.TestCase):
    def test_preserves_values_without_game_names(self):
        self.assertEqual(stubs.brightness_rows('// header\n{ 12, 0, 1, -1 }, // secret map\n{ 15, 2, 3, 1 },'),
                         ['BRIGHT\t12\t0\t1\t-1', 'BRIGHT\t15\t2\t3\t1'])

    def test_rejects_empty_overflow_and_capacity_mismatch(self):
        for text in ('', '{ 65536, 0, 1, 0 },', '{ 1, 0, 1, 128 },', '{ 1, 0, 1, 0 },\n' * 4097):
            with self.subTest(text=text[:32]), self.assertRaises(AssertionError):
                stubs.brightness_rows(text)


if __name__ == '__main__':
    unittest.main()
