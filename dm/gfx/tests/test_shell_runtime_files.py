from pathlib import Path
import contextlib
import importlib.util
import io
import tempfile
import unittest
import zipfile

DM = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('d2rm_pack', DM / 'tools/d2rm_pack.py')
pack = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pack)


class ShellRuntimeTablesTest(unittest.TestCase):
    def test_rejects_missing_runtime_table_before_packaging(self):
        for missing in ('shader_map.tsv', 'pso_manifest.tsv', None):
            with self.subTest(missing=missing), tempfile.TemporaryDirectory(dir=Path(__file__).parent) as directory:
                ipa = Path(directory) / 'shell.ipa'
                base = 'Payload/DmIOS.app/'
                with zipfile.ZipFile(ipa, 'w') as z:
                    z.writestr(base + 'dm/shell_manifest.txt', 'dm/derived.tsv\n')
                    z.writestr(base + 'dm/shell_info.txt', 'PRODUCT=test\nCHAIN=test\n')
                    z.writestr(base + 'DmIOS', b'fixture')
                    for name in ('shader_map.tsv', 'pso_manifest.tsv'):
                        if name != missing: z.writestr(base + 'dm/' + name, b'fixture')
                if missing:
                    with contextlib.redirect_stdout(io.StringIO()), self.assertRaises(SystemExit): pack.read_shell(str(ipa))
                else:
                    result = pack.read_shell(str(ipa))
                    result[0].close()


if __name__ == '__main__':
    unittest.main()
