# ============================================================
# NOUVEAU - MiniBank 3.9.4.8 : verification globale C++17
# PowerShell 5.1 / PowerShell 7 - Windows / MSYS2 g++
# Lecture Git uniquement. Aucun nettoyage, commit ou push.
# ============================================================

$ErrorActionPreference = 'Stop'
$racine = 'C:\Users\saide\Desktop\ISOSET\SaZ-Projet_Cpp\MiniBank_Cpp'
Set-Location -LiteralPath $racine

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw 'Compilateur g++ introuvable dans PATH.'
}
if (-not (Test-Path -LiteralPath 'data' -PathType Container)) {
    throw 'Dossier data introuvable. Aucun dossier de verification cree.'
}

# NOUVEAU : repertoire neuf, sans toucher aux sauvegardes reelles.
$id = [guid]::NewGuid().ToString('N')
$travail = Join-Path $racine ("data\verification_3948_$id")
$bin = Join-Path $travail 'bin'
$scenarios = Join-Path $travail 'scenarios'
$journaux = Join-Path $travail 'journaux'
New-Item -ItemType Directory -Path $travail -ErrorAction Stop | Out-Null
foreach ($dossier in @($bin, $scenarios, $journaux)) {
    New-Item -ItemType Directory -Path $dossier -ErrorAction Stop | Out-Null
}

# NOUVEAU : sources et suites de reference.
$sources = @(
    'src\Compte.cpp', 'src\Banque.cpp', 'src\Montant.cpp',
    'src\Persistance.cpp', 'src\Deserialisation.cpp',
    'src\Chargement.cpp', 'src\Restauration.cpp',
    'src\Recuperation.cpp'
)
$suites = @(
    @{ Nom='test_compte';                    Dossier=$false },
    @{ Nom='test_banque';                    Dossier=$false },
    @{ Nom='test_identifiants';              Dossier=$false },
    @{ Nom='test_montant';                   Dossier=$false },
    @{ Nom='test_parcours';                  Dossier=$false },
    @{ Nom='test_virement';                  Dossier=$false },
    @{ Nom='test_historique';                Dossier=$false },
    @{ Nom='test_liste_comptes';             Dossier=$false },
    @{ Nom='test_serialisation';            Dossier=$false },
    @{ Nom='test_ecriture_temporaire';       Dossier=$true  },
    @{ Nom='test_publication_windows';       Dossier=$true  },
    @{ Nom='test_donnees_invalides';         Dossier=$true  },
    @{ Nom='test_deserialisation';           Dossier=$false },
    @{ Nom='test_validation_deserialisation';Dossier=$false },
    @{ Nom='test_chargement';                Dossier=$true  },
    @{ Nom='test_restauration';              Dossier=$false },
    @{ Nom='test_recuperation';              Dossier=$true  },
    @{ Nom='test_integration_restauration';  Dossier=$true  }
)

# NOUVEAU : execution native journalisee, avec code retour fiable.
# On evite l'effet de ErrorActionPreference=Stop sur stderr de g++
# sous Windows PowerShell 5.1.
function Executer-Natif {
    param(
        [Parameter(Mandatory=$true)][string]$Programme,
        [string[]]$Arguments = @(),
        [Parameter(Mandatory=$true)][string]$Journal
    )
    $anciennePreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        & $Programme @Arguments 2>&1 | Out-File -LiteralPath $Journal -Encoding UTF8
        $code = $LASTEXITCODE
    }
    finally {
        $ErrorActionPreference = $anciennePreference
    }
    if ($null -eq $code) { return 999 }
    return [int]$code
}

# NOUVEAU : compter uniquement ce qui est effectivement observable.
function Analyser-Journal {
    param([Parameter(Mandatory=$true)][string]$Chemin)
    $lignes = @(Get-Content -LiteralPath $Chemin -ErrorAction Stop)
    $ok = @($lignes | Where-Object { $_ -match '^\s*\[OK\]' }).Count
    $echecs = @($lignes | Where-Object {
        $_ -match '^\s*\[ECHEC\]' -or
        $_ -match '^\s*Tests?\s+echoues\s*:\s*[1-9]\d*\s*$' -or
        $_ -match '^\s*(?:Tests?\s+)?FAIL(?:ED)?\s*:\s*[1-9]\d*\s*$'
    }).Count
    $executes = $null
    $reussis = $null
    $echoues = $null
    foreach ($ligne in $lignes) {
        if ($ligne -match '^\s*Tests?\s+executes\s*:\s*(\d+)\s*$') {
            $executes = [int]$Matches[1]
        }
        if ($ligne -match '^\s*Tests?\s+reussis\s*:\s*(\d+)\s*$') {
            $reussis = [int]$Matches[1]
        }
        if ($ligne -match '^\s*Tests?\s+echoues\s*:\s*(\d+)\s*$') {
            $echoues = [int]$Matches[1]
        }
    }
    $coherent = $true
    if ($null -ne $executes -and $null -ne $reussis -and $null -ne $echoues) {
        $coherent = ($executes -eq ($reussis + $echoues))
    }
    if ($null -ne $echoues -and $echoues -gt 0) { $echecs++ }
    return [pscustomobject]@{
        TestsDeclares = $executes
        ReussisDeclares = $reussis
        EchouesDeclares = $echoues
        MarqueursOK = $ok
        SignauxEchec = $echecs
        CompteursCoherents = $coherent
    }
}

$resultats = [System.Collections.Generic.List[object]]::new()
$compilationGenerale = $false
$erreurGenerale = ''
$debut = Get-Date

try {
    Write-Host "`n=== MiniBank 3.9.4.8 : $travail ===" -ForegroundColor Cyan
    foreach ($fichier in @('src\main.cpp') + $sources) {
        if (-not (Test-Path -LiteralPath $fichier -PathType Leaf)) {
            throw "Source manquante : $fichier"
        }
    }
    foreach ($suite in $suites) {
        $test = "tests\$($suite.Nom).cpp"
        if (-not (Test-Path -LiteralPath $test -PathType Leaf)) {
            throw "Test manquant : $test"
        }
    }

    # NOUVEAU : controle de compilation de l'application.
    $application = Join-Path $travail 'MiniBank.exe'
    $journalApp = Join-Path $journaux 'MiniBank_compilation.txt'
    $argumentsApp = @('-std=c++17','-Wall','-Wextra','-Iinclude','src\main.cpp') +
                    $sources + @('-o', $application)
    Write-Host 'Compilation generale de MiniBank.exe...' -ForegroundColor Cyan
    $codeApp = Executer-Natif -Programme 'g++' -Arguments $argumentsApp -Journal $journalApp
    $compilationGenerale = ($codeApp -eq 0 -and (Test-Path -LiteralPath $application -PathType Leaf))
    if (-not $compilationGenerale) {
        $erreurGenerale = "Compilation generale echouee (code $codeApp). Journal : $journalApp"
        Write-Host $erreurGenerale -ForegroundColor Red
    }

    # NOUVEAU : executer toutes les suites, meme si une autre echoue.
    foreach ($suite in $suites) {
        $nom = $suite.Nom
        Write-Host "`n=== $nom ===" -ForegroundColor Cyan
        $exe = Join-Path $bin "$nom.exe"
        $logCompilation = Join-Path $journaux "${nom}_compilation.txt"
        $logExecution = Join-Path $journaux "${nom}_execution.txt"
        $argsCompilation = @('-std=c++17','-Wall','-Wextra','-Iinclude',
                             "tests\$nom.cpp") + $sources + @('-o', $exe)
        $codeComp = Executer-Natif -Programme 'g++' -Arguments $argsCompilation -Journal $logCompilation
        $compile = ($codeComp -eq 0 -and (Test-Path -LiteralPath $exe -PathType Leaf))
        $codeTest = $null
        $analyse = $null
        $etat = 'COMPILATION_ECHOUEE'
        if ($compile) {
            $argsTest = @()
            if ($suite.Dossier) {
                $dossierTest = Join-Path $scenarios $nom
                New-Item -ItemType Directory -Path $dossierTest -ErrorAction Stop | Out-Null
                $argsTest = @($dossierTest)
            }
            $codeTest = Executer-Natif -Programme $exe -Arguments $argsTest -Journal $logExecution
            $analyse = Analyser-Journal -Chemin $logExecution
            if ($codeTest -eq 0 -and $analyse.SignauxEchec -eq 0 -and $analyse.CompteursCoherents) {
                $etat = 'REUSSIE'
            } else {
                $etat = 'TEST_ECHOUE'
            }
        }
        $testsDeclares = $null
        $reussisDeclares = $null
        $marqueursOK = $null
        if ($null -ne $analyse) {
            $testsDeclares = $analyse.TestsDeclares
            $reussisDeclares = $analyse.ReussisDeclares
            $marqueursOK = $analyse.MarqueursOK
        }
        $resultats.Add([pscustomobject]@{
            Suite=$nom
            Compilation=$compile
            CodeCompilation=$codeComp
            CodeExecution=$codeTest
            Etat=$etat
            TestsDeclares=$testsDeclares
            ReussisDeclares=$reussisDeclares
            MarqueursOK=$marqueursOK
            JournalCompilation=$logCompilation
            JournalExecution=$logExecution
        })
        Write-Host "$nom : $etat" -ForegroundColor $(if ($etat -eq 'REUSSIE') {'Green'} else {'Red'})
    }
}
catch {
    $erreurGenerale = $_.Exception.Message
    Write-Host "ERREUR DE PREPARATION : $erreurGenerale" -ForegroundColor Red
}
finally {
    # NOUVEAU : bilan et journaux meme en cas de panne partielle.
    $csv = Join-Path $travail 'bilan.csv'
    $txt = Join-Path $travail 'bilan.txt'
    $resultats | Export-Csv -LiteralPath $csv -NoTypeInformation -Encoding UTF8
    $compilationsOK = @($resultats | Where-Object Compilation).Count
    $suitesOK = @($resultats | Where-Object { $_.Etat -eq 'REUSSIE' }).Count
    $avecCompteur = @($resultats | Where-Object { $null -ne $_.TestsDeclares })
    $sansCompteur = @($resultats | Where-Object { $null -eq $_.TestsDeclares })
    $totalConnu = 0
    foreach ($r in $avecCompteur) { $totalConnu += [int]$r.TestsDeclares }
    $succesGlobal = ($compilationGenerale -and $compilationsOK -eq 18 -and
                     $suitesOK -eq 18 -and [string]::IsNullOrEmpty($erreurGenerale))
    $bilan = @(
        'MINIBANK - BILAN 3.9.4.8',
        "Dossier : $travail",
        "Compilation generale : $compilationGenerale",
        "Compilations de suites : $compilationsOK / 18",
        "Suites reussies : $suitesOK / 18",
        "Assertions declarees, total partiel : $totalConnu",
        "Suites avec compteur explicite : $($avecCompteur.Count) / 18",
        "Suites sans compteur explicite : $($sansCompteur.Count) / 18",
        'Reference historique des 15 premieres suites : 773 (non ajoutee au total partiel)',
        'Integration 3.9.4.7 : 47 tests declares lors de sa validation precedente',
        "Succes global : $succesGlobal",
        "Erreur de preparation : $erreurGenerale",
        "Debut : $debut",
        "Fin : $(Get-Date)"
    )
    $bilan | Set-Content -LiteralPath $txt -Encoding UTF8
    Write-Host "`n========================================" -ForegroundColor Cyan
    $bilan | ForEach-Object { Write-Host $_ }
    Write-Host '========================================' -ForegroundColor Cyan
    $resultats | Format-Table Suite,Compilation,Etat,TestsDeclares,MarqueursOK -AutoSize
    Write-Host "Journaux : $journaux"
    Write-Host "Bilan CSV : $csv"
    Write-Host "Bilan TXT : $txt"
    Write-Host "`nEtat Git (lecture seule) :"
    & git status --short
    if (-not $succesGlobal) {
        Write-Host 'VERIFICATION NON VALIDEE. Consulter les journaux.' -ForegroundColor Red
        throw 'MiniBank 3.9.4.8 : verification globale non validee.'
    }
    Write-Host 'VERIFICATION GLOBALE REUSSIE : 18/18.' -ForegroundColor Green
}
