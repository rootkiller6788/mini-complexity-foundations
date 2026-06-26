# Ralph Wiggum Loop — mini-cook-levin-theorem (PowerShell)
# 用法: cd 到本目录, 然后 .\ralph.ps1

$MAX_ITER = 3
$PROMPT = "F:\nano-everything\mini-theory-of-computation\SKILL.md"
$TARGET = "F:\nano-everything\mini-theory-of-computation\0. mini-complexity-foundations\mini-cook-levin-theorem"

Write-Host "============================================"
Write-Host "  Ralph Loop — mini-cook-levin-theorem"
Write-Host "  SKILL: $PROMPT"
Write-Host "  最大迭代: $MAX_ITER"
Write-Host "============================================"

for ($i = 1; $i -le $MAX_ITER; $i++) {
    Write-Host "`n══════════ 第 $i / $MAX_ITER 轮 ══════════"
    
    $before = (Get-ChildItem "$TARGET\include\*.h", "$TARGET\src\*.c" -ErrorAction SilentlyContinue | Get-Content | Measure-Object -Line).Lines
    Write-Host "  迭代前行数: $before"
    
    Set-Location $TARGET
    claude -p "$(Get-Content $PROMPT -Raw)"
    
    $after = (Get-ChildItem "include\*.h", "src\*.c" -ErrorAction SilentlyContinue | Get-Content | Measure-Object -Line).Lines
    Write-Host "  迭代后行数: $after (+$($after - $before))"
    
    if ((Get-Content README.md -Raw -ErrorAction SilentlyContinue) -match "COMPLETE") {
        Write-Host "`n✅ 检测到 COMPLETE 信号! 循环终止。"
        break
    }
    
    if ($after -ge 3000) {
        Write-Host "  行数达标 (>=3000)，但 README.md 未标记 COMPLETE"
    }
}

Write-Host "`n===== 最终状态 ====="
$final = (Get-ChildItem "$TARGET\include\*.h", "$TARGET\src\*.c" -ErrorAction SilentlyContinue | Get-Content | Measure-Object -Line).Lines
Write-Host "  include+src 总行数: $final / 3000"
