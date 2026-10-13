from pathlib import Path
import importlib.util
import plistlib
import struct
import tempfile
import unittest

DM = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('guest_dylib', DM / 'tools/guest_dylib.py')
guest = importlib.util.module_from_spec(spec)
spec.loader.exec_module(guest)


class IOS18ArtifactsTest(unittest.TestCase):
    def test_guest_generated_library_and_plist_match_ios18_target(self):
        with tempfile.TemporaryDirectory(dir=Path(__file__).resolve().parent) as directory:
            root = Path(directory)
            (root / 'main.fixups.tsv').write_text('# TEXT_END 0x4000\n# PATCH_OFF 0x8000\n')
            (root / 'main.patched.bin').write_bytes(b'\0' * 0xc000)
            framework, size = guest.write(str(root), str(root / 'out'), ['main'])
            binary = (Path(framework) / 'Guest').read_bytes()
            self.assertEqual(len(binary), size)
            command = 32
            versions = []
            for _ in range(struct.unpack_from('<I', binary, 16)[0]):
                kind, length = struct.unpack_from('<II', binary, command)
                if kind == 0x32:
                    versions.append(struct.unpack_from('<I', binary, command + 12)[0])
                command += length
            self.assertEqual(versions, [(18 << 16) | (4 << 8)])
            info = plistlib.loads((Path(framework) / 'Info.plist').read_bytes())
            self.assertEqual(info['MinimumOSVersion'], '18.4')

    def test_production_renderer_has_no_metal4_api_dependency(self):
        import re
        files = [DM / 'gfx/nvn/nvn_mtl.m', DM / 'gfx/tex/nvn_deswizzle_gpu.m', DM / 'gfx/tex/nvn_deswizzle_gpu.h']
        files.extend((DM / 'gfx/nvn').glob('*.inc'))
        for file in files:
            with self.subTest(file=file.name):
                self.assertFalse(re.findall(r'\bMTL4\w+', file.read_text(encoding='utf-8')))

    def test_project_uses_ios18_and_manual_resource_import(self):
        project = (DM / 'ios/project.yml').read_text(encoding='utf-8')
        self.assertIn('iOS: "18.4"', project)
        self.assertNotIn('RomfsDownloader', project)
        main = (DM / 'ios/Sources/main.m').read_text(encoding='utf-8')
        self.assertNotIn('BAAssetPackManager.sharedManager', main)
        self.assertNotIn('<BAManagedAssetPackDownloadDelegate>', main)


if __name__ == '__main__':
    unittest.main()
