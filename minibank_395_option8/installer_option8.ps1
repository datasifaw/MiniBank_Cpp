# NOUVEAU : installation prudente de l'option 8, sans reconstruire main.cpp.
$ErrorActionPreference = 'Stop'
$racine = (Get-Location).Path
$main = Join-Path $racine 'src\main.cpp'
$enteteSource = Join-Path $PSScriptRoot 'include\MenuPersistance.h'
$enteteDestination = Join-Path $racine 'include\MenuPersistance.h'
if (-not (Test-Path -LiteralPath $main -PathType Leaf)) { throw 'src/main.cpp introuvable.' }
if (-not (Test-Path -LiteralPath $enteteSource -PathType Leaf)) { throw 'include/MenuPersistance.h introuvable dans le pack.' }
if (Test-Path -LiteralPath $enteteDestination) { throw 'MenuPersistance.h existe deja : aucune modification.' }
$utf8 = New-Object System.Text.UTF8Encoding($false)
$contenu = [System.IO.File]::ReadAllText($main, [System.Text.Encoding]::UTF8)
$motif = 'case\s+8\s*:\s*\{\s*std::cout\s*<<\s*"Fonctionnalite a venir !"\s*<<\s*std::endl\s*;\s*break\s*;\s*\}'
$expression = New-Object System.Text.RegularExpressions.Regex($motif)
if ($expression.Matches($contenu).Count -ne 1) { throw 'Bloc option 8 different de la capture. Aucun fichier modifie.' }
if ($contenu.Contains('"MenuPersistance.h"')) { throw 'Include deja present. Aucun fichier modifie.' }
$includePattern = '(?m)^\s*#include\s*[<"][^>"\r\n]+[>"]\s*\r?\n'
$includes = [regex]::Matches($contenu, $includePattern)
if ($includes.Count -eq 0) { throw 'Aucun include detecte. Aucun fichier modifie.' }
$dernier = $includes[$includes.Count - 1]
$position = $dernier.Index + $dernier.Length
$nouveau = $contenu.Insert($position, '#include "MenuPersistance.h"' + [Environment]::NewLine)
$nouveau = $expression.Replace($nouveau, "case 8: {`r`n            // NOUVEAU : ouvrir le sous-menu sans toucher aux autres options.`r`n            menu_persistance::afficher(banque);`r`n            break;`r`n        }")
# Ne pas modifier avant d'avoir confirme que la variable banque existe.
if ($nouveau -notmatch '\bBanque\s+banque\b') { throw 'Variable Banque banque introuvable. Aucun fichier modifie.' }
$dossierBackup = Join-Path $racine 'data'
if (-not (Test-Path -LiteralPath $dossierBackup -PathType Container)) { throw 'Dossier data absent. Aucun fichier modifie.' }
$backup = Join-Path $dossierBackup ('main_avant_option8_' + [guid]::NewGuid().ToString('N') + '.cpp')
[System.IO.File]::Copy($main, $backup)
try {
    [System.IO.File]::Copy($enteteSource, $enteteDestination)
    [System.IO.File]::WriteAllText($main, $nouveau, $utf8)
} catch {
    [System.IO.File]::Copy($backup, $main, $true)
    if (Test-Path -LiteralPath $enteteDestination) { Remove-Item -LiteralPath $enteteDestination }
    throw
}
Write-Host 'Option 8 integree : main.cpp conserve, module ajoute.' -ForegroundColor Green
Write-Host "Copie de securite : $backup"
Write-Host 'Aucun commit ni push realise.'
