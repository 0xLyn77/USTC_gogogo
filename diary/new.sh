#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
TODAY=$(date +%Y-%m-%d)
TARGET_FILE="$SCRIPT_DIR/$TODAY.md"

if [ -e "$TARGET_FILE" ]; then
    printf '文件已存在，跳过创建: %s\n' "$TARGET_FILE"
    exit 0
fi

cat <<EOF > "$TARGET_FILE"
# $TODAY

## 今日复盘

## 今日未做

## 明日计划
EOF

printf '已创建日记: %s\n' "$TARGET_FILE"
