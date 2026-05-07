import cv2
import numpy as np
from dataclasses import dataclass
from dir import Dir
from maze import Maze

# ============================================================
# 1. 便利関数：画像を二値化して「壁=1, 空白=0」に統一する
# ============================================================
def __binarize_wall_image(img_bgr):
    """
    入力画像（カラー）を二値化して、壁=1, 床=0 の画像を返す。
    """
    gray = cv2.cvtColor(img_bgr, cv2.COLOR_BGR2GRAY)

    # Otsuで二値化
    _, bw = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

    # bw は普通「床が白(255), 壁が黒(0)」になることが多いので、
    # 壁=255に揃えるため反転する
    bw = cv2.bitwise_not(bw)

    # 0/1に変換
    bw01 = (bw > 0).astype(np.uint8)
    return bw01


# ============================================================
# 2. 便利関数：迷路領域の外枠を見つけてトリミングする
# ============================================================
def __crop_maze_region(bw01):
    """
    二値画像(bw01)から迷路外枠を探して、迷路部分だけ切り出す。
    """
    # 輪郭抽出
    contours, _ = cv2.findContours(bw01, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(contours) == 0:
        raise RuntimeError("No contour found in maze image.")

    # 最大の輪郭を迷路外枠とみなす
    c = max(contours, key=cv2.contourArea)

    x, y, w, h = cv2.boundingRect(c)
    cropped = bw01[y:y+h, x:x+w]

    return cropped


# ============================================================
# 3. 迷路サイズ検出（縦の区切り線本数を数える）
# ============================================================
def detect_maze_size(trim):
    """
    正規化された迷路画像から迷路サイズNを推定する。

    ここでは「縦方向の壁の周期性」を使って推定する。
    """
    colsum = np.sum(trim, axis=0)
    avg = np.mean(colsum)

    # 壁がある列は colsum が大きいので、閾値で0/1化
    bin_line = (colsum > avg).astype(np.uint8)

    # 連続区間の立ち上がり回数を数える
    diff = np.diff(np.concatenate([[0], bin_line, [0]]))
    peaks = np.sum(diff == 1)

    # 外壁も含めた「縦線の本数」は N+1 本になる
    # なので N = peaks - 1
    N = peaks - 1

    if N <= 1:
        raise RuntimeError("Failed to detect maze size.")
    return N


# ============================================================
# 4. 壁判定の本体：矩形領域の黒率で判定する
# ============================================================
def region_wall_ratio(img, x0, y0, x1, y1):
    """
    img(0/1) の矩形領域 [y0:y1, x0:x1] の平均値（黒率）を返す。
    """
    H, W = img.shape

    # 範囲クリップ
    x0 = max(0, min(W - 1, x0))
    x1 = max(0, min(W, x1))
    y0 = max(0, min(H - 1, y0))
    y1 = max(0, min(H, y1))

    if x1 <= x0 or y1 <= y0:
        return 0.0

    roi = img[y0:y1, x0:x1]
    return float(np.mean(roi))


# ============================================================
# 5. 壁配列生成：境界線を矩形領域として調べる
# ============================================================
def extract_walls(trim, N, threshold=0.25):
    """
    trim : 正規化済みの迷路二値画像(0/1)
    N    : maze size
    threshold : 黒率がこれ以上なら壁と判定する

    出力:
      vwall (N, N+1)
      hwall (N+1, N)
    """
    H, W = trim.shape
    seg_h = H / N
    seg_w = W / N

    # vwall: 縦壁 (N行, N+1列)
    vwall = np.zeros((N, N + 1), dtype=np.uint8)

    # hwall: 横壁 (N+1行, N列)
    hwall = np.zeros((N + 1, N), dtype=np.uint8)

    # 境界線を検出する矩形の太さ
    # 境界線は太線/細線が混ざるので、ある程度厚めに取る
    band = int(min(seg_h, seg_w) * 0.15)
    if band < 2:
        band = 2

    # ------------------------------------------------------------
    # vwall抽出
    #
    # vwall[y, xb] は y行目セルの縦境界 xb を表す
    # xb=0 が一番左外壁, xb=N が一番右外壁
    #
    # 各境界線について、縦方向の細長い矩形領域を見て黒率判定する
    # ------------------------------------------------------------
    for y in range(N):
        y_center = int((y + 0.5) * seg_h)

        for xb in range(N + 1):
            x_boundary = int(xb * seg_w)

            # 境界線周辺の矩形領域を切り出す
            x0 = x_boundary - band
            x1 = x_boundary + band
            y0 = y_center - int(seg_h * 0.4)
            y1 = y_center + int(seg_h * 0.4)

            ratio = region_wall_ratio(trim, x0, y0, x1, y1)

            if ratio > threshold:
                vwall[y, xb] = 1

    # ------------------------------------------------------------
    # hwall抽出
    #
    # hwall[yb, x] は x列目セルの横境界 yb を表す
    # yb=0 が一番下外壁, yb=N が一番上外壁
    #
    # 各境界線について、横方向の細長い矩形領域を見て黒率判定する
    # ------------------------------------------------------------
    for yb in range(N + 1):
        y_boundary = int(yb * seg_h)

        for x in range(N):
            x_center = int((x + 0.5) * seg_w)

            x0 = x_center - int(seg_w * 0.4)
            x1 = x_center + int(seg_w * 0.4)
            y0 = y_boundary - band
            y1 = y_boundary + band

            ratio = region_wall_ratio(trim, x0, y0, x1, y1)

            if ratio > threshold:
                hwall[yb, x] = 1

    # ------------------------------------------------------------
    # 重要：座標系を合わせる
    #
    # OpenCV画像は y=0 が上。
    # 迷路座標は y=0 が下。
    #
    # vwallはセル行に対応するので flipud する
    # hwallは境界行なので flipud する（N+1行なのでOK）
    # ------------------------------------------------------------
    vwall = np.flipud(vwall)
    hwall = np.flipud(hwall)

    return vwall.astype(bool), hwall.astype(bool)



# ============================================================
# 6. メイン関数：迷路画像からMaze構造体を生成
# ============================================================
def parse_maze_image(filename: str, normalize_size: int = 1600) -> Maze:
    """
    迷路画像から壁情報を抽出して Maze を返す。
    """

    # ------------------------------------------------------------
    # 画像読み込み
    # ------------------------------------------------------------
    original = cv2.imread(filename, cv2.IMREAD_COLOR)
    if original is None:
        raise FileNotFoundError(f"File not found or cannot read: {filename}")

    # ------------------------------------------------------------
    # 二値化（壁=1）
    # ------------------------------------------------------------
    bw01 = __binarize_wall_image(original)

    # ------------------------------------------------------------
    # 外枠でトリミング
    # ------------------------------------------------------------
    trim = __crop_maze_region(bw01)

    # ------------------------------------------------------------
    # サイズ正規化
    # ------------------------------------------------------------
    trim = cv2.resize(trim, (normalize_size, normalize_size), interpolation=cv2.INTER_NEAREST)

    # ------------------------------------------------------------
    # ノイズ除去（点線グリッドなどを減らす）
    # ------------------------------------------------------------
    trim = cv2.morphologyEx(trim, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
    trim = cv2.dilate(trim, np.ones((3, 3), np.uint8), iterations=1)

    trim = (trim > 0).astype(np.uint8)

    # ------------------------------------------------------------
    # 迷路サイズ検出
    # ------------------------------------------------------------
    N = detect_maze_size(trim)

    # Validate maze size (must be 16x16 for C solver compatibility)
    if N != 16:
        raise ValueError(f"Maze must be 16×16, got {N}×{N}")

    # ------------------------------------------------------------
    # 壁抽出（矩形領域判定）
    # ------------------------------------------------------------
    vwall_bool, hwall_bool = extract_walls(trim, N, threshold=0.20)


    return Maze(
        size=N,
        vwall=vwall_bool,
        hwall=hwall_bool,
    )



if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python parse_maze_image.py <maze_image.png>")
        sys.exit(0)

    maze = parse_maze_image(sys.argv[1])

    print(maze)
    maze.print_maze_ascii()