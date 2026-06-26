#!/bin/bash
# Ralph Wiggum Loop — mini-cook-levin-theorem v2
# 用法: cd 到本目录, 然后 bash ralph.sh

MAX_ITER=2
TIMEOUT_SEC=300  # 每轮最长时间 (10分钟)
PROMPT="F:/nano-everything/mini-theory-of-computation/0. mini-complexity-foundations/mini-cook-levin-theorem/PROMPT.md"
TARGET="F:/nano-everything/mini-theory-of-computation/0. mini-complexity-foundations/mini-cook-levin-theorem"

count_lines() {
    (cd "$TARGET" && find include src -name "*.c" -o -name "*.h" 2>/dev/null | xargs cat 2>/dev/null | wc -l)
}

echo "============================================"
echo "  Ralph Loop — mini-cook-levin-theorem v2"
echo "  SKILL: $PROMPT"
echo "  最大迭代: $MAX_ITER | 单轮超时: ${TIMEOUT_SEC}s"
echo "============================================"
echo ""

for i in $(seq 1 $MAX_ITER); do
    echo "══════════ 第 $i / $MAX_ITER 轮 ══════════"
    
    before=$(count_lines)
    echo "  迭代前行数: $before"
    
    cd "$TARGET"
    timeout $TIMEOUT_SEC claude -p "$(cat "$PROMPT")"
    exit_code=$?
    
    after=$(count_lines)
    delta=$((after - before))
    echo "  迭代后行数: $after ($( [ $delta -ge 0 ] && echo "+" )$delta)"
    
    if [ $exit_code -eq 124 ]; then
        echo "  ⚠️ 超时 (${TIMEOUT_SEC}s)，进入下一轮"
    fi
    
    if grep -q "COMPLETE" "$TARGET/README.md" 2>/dev/null; then
        echo ""
        echo "✅ 检测到 COMPLETE! 循环终止。"
        break
    fi
    
    if [ "$after" -ge 3000 ]; then
        echo "  行数达标 (≥3000)"
    fi
    
    echo ""
done

echo ""
echo "===== 最终状态 ====="
final=$(count_lines)
echo "  include+src: $final / 3000 行"
echo "  README.md:  $( [ -f "$TARGET/README.md" ] && echo "存在" || echo "缺失" )"
echo "  COMPLETE:   $(grep -c 'COMPLETE' "$TARGET/README.md" 2>/dev/null || echo 0) 处"
echo "  make:       $(cd "$TARGET" && make lib 2>&1 | grep -c '✅')"
