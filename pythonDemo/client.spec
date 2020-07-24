# -*- mode: python ; coding: utf-8 -*-

block_cipher = None


a = Analysis(['client.py'],
             pathex=['/home/fschmid/Applications/privacy-preserving-disease-analysis/pythonDemo/venv/lib/python3.7/site-packages'],
             binaries=[],
             datas=[
                ("./venv/lib/python3.7/site-packages/branca/*.json","branca"),
                ("./venv/lib/python3.7/site-packages/branca/templates","templates"),
                ("./venv/lib/python3.7/site-packages/folium/templates","templates"),
             ],
             hiddenimports=['/home/fschmid/Applications/privacy-preserving-disease-analysis/pythonDemo/venv/lib/python3.7/site-packages/pkg_resources.py2_warn'],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
         cipher=block_cipher)
exe = EXE(pyz,
      a.scripts,
      a.binaries,
      a.zipfiles,
      a.datas,
      [],
      name='client',
      debug=False,
      bootloader_ignore_signals=False,
      strip=False,
      upx=True,
      runtime_tmpdir=None,
      console=True )