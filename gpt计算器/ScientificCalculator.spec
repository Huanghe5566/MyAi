# -*- mode: python ; coding: utf-8 -*-


python_root = r"C:\Users\BLWH-BK-0214\AppData\Local\Programs\Python\Python311"

a = Analysis(
    ["scientific_calculator.py"],
    pathex=[],
    binaries=[
        (python_root + r"\DLLs\tcl86t.dll", "."),
        (python_root + r"\DLLs\tk86t.dll", "."),
    ],
    datas=[
        (python_root + r"\tcl\tcl8.6", "_tcl_data"),
        (python_root + r"\tcl\tk8.6", "_tk_data"),
        (python_root + r"\tcl\tcl8", "tcl8"),
    ],
    hiddenimports=["tkinter", "_tkinter"],
    hookspath=["hooks"],
    hooksconfig={},
    runtime_hooks=["runtime_tkinter.py"],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name="ScientificCalculator",
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name="ScientificCalculator",
)
