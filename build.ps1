#requires -Version 5.1
<#
  MidiForge - script de compilación para Windows.
  - Comprueba CMake y Visual Studio.
  - Opcionalmente instala lo que falte con winget (si el usuario lo aprueba).
  - Configura y compila el VST3.
#>

param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",
    [switch]$InstallPrereqs
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path

function Test-Cmd([string]$name) {
    return [bool](Get-Command $name -ErrorAction SilentlyContinue)
}

function Find-CMake {
    if (Test-Cmd "cmake") { return "cmake" }
    $candidates = @(
        "C:\Program Files\CMake\bin\cmake.exe",
        "C:\Program Files (x86)\CMake\bin\cmake.exe",
        "$env:LOCALAPPDATA\Programs\CMake\bin\cmake.exe"
    )
    foreach ($c in $candidates) { if (Test-Path $c) { return $c } }
    return $null
}

function Find-VS {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere) {
        $path = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($path) { return $path.Trim() }
    }
    return $null
}

Write-Host "== MidiForge build ==" -ForegroundColor Cyan

$cmake = Find-CMake
$vs = Find-VS

if (-not $cmake) {
    Write-Warning "CMake no encontrado."
    if ($InstallPrereqs -and (Test-Cmd "winget")) {
        Write-Host "Instalando CMake con winget..." -ForegroundColor Yellow
        winget install --id Kitware.CMake -e --accept-source-agreements --accept-package-agreements
        $cmake = Find-CMake
    }
}
if (-not $vs) {
    Write-Warning "Visual Studio con C++ no encontrado."
    if ($InstallPrereqs -and (Test-Cmd "winget")) {
        Write-Host "Instalando Visual Studio Build Tools (C++) con winget..." -ForegroundColor Yellow
        winget install --id Microsoft.VisualStudio.2022.BuildTools -e `
            --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" `
            --accept-source-agreements --accept-package-agreements
        $vs = Find-VS
    }
}

if (-not $cmake) {
    throw "CMake es obligatorio. Instálalo y vuelve a ejecutar (o usa -InstallPrereqs)."
}
if (-not $vs) {
    throw "Visual Studio (Desktop C++) es obligatorio. Instálalo y vuelve a ejecutar (o usa -InstallPrereqs)."
}

Write-Host "CMake : $cmake"
Write-Host "VS    : $vs"

Write-Host "`nConfigurando..." -ForegroundColor Cyan
& $cmake -B "$root\build" -G "Visual Studio 17 2022" -A x64 -S $root
if ($LASTEXITCODE -ne 0) { throw "Fallo en la configuración de CMake." }

Write-Host "`nCompilando ($Config)..." -ForegroundColor Cyan
& $cmake --build "$root\build" --config $Config --parallel
if ($LASTEXITCODE -ne 0) { throw "Fallo en la compilación." }

$vst3 = Get-ChildItem -Path "$root\build" -Recurse -Directory -Filter "MidiForge.vst3" -ErrorAction SilentlyContinue | Select-Object -First 1
if ($vst3) {
    Write-Host "`nOK. Plugin generado en:" -ForegroundColor Green
    Write-Host "  $($vst3.FullName)"
    Write-Host "`nCopiar a C:\Program Files\Common Files\VST3\ (como administrador) y escanear en FL Studio." -ForegroundColor Green
} else {
    Write-Host "`nCompilación terminada, pero no se encontró MidiForge.vst3. Revisa la salida." -ForegroundColor Yellow
}
