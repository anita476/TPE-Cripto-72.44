# metrics.py
from math import log10, sqrt
import os
import glob
import cv2
import numpy as np
import pandas as pd

# ---------- config ----------
RESOURCES_DIR = "resources"
OUT_DIR = "out"
VARIANTS = ["lsb1", "lsb4", "lsbi"]

# ---------- utilidades ----------
def load_image(path):
    img = cv2.imread(path, cv2.IMREAD_COLOR)
    if img is None:
        raise FileNotFoundError(f"No pude leer la imagen: {path}")
    return img

def psnr(original, compressed):
    orig = original.astype(np.float64)
    comp = compressed.astype(np.float64)
    mse = np.mean((orig - comp) ** 2)
    if mse == 0:
        return float("inf")
    return 20.0 * log10(255.0 / sqrt(mse))

def mse(original, compressed):
    """Mean Squared Error"""
    orig = original.astype(np.float64)
    comp = compressed.astype(np.float64)
    return float(np.mean((orig - comp) ** 2))

def shannon_entropy(img):
    """Entropía en bits sobre todos los canales (0..255)."""
    arr = img.ravel()
    hist = np.bincount(arr, minlength=256).astype(np.float64)
    p = hist / hist.sum()
    p = p[p > 0]
    return float(-(p * np.log2(p)).sum())

def lsb_entropy(img):
    """Entropía de los LSBs (0/1) en todos los bytes."""
    lsb = (img & 1).ravel()
    counts = np.bincount(lsb, minlength=2).astype(np.float64)
    p = counts / counts.sum()
    p = p[p > 0]
    return float(-(p * np.log2(p)).sum())

def lsb_entropy_blocks(img, block_size=64):
    """
    Calcula entropía LSB en bloques.
    Retorna (promedio, desviación estándar)
    """
    h, w = img.shape[:2]
    entropies = []
    
    for y in range(0, h - block_size + 1, block_size):
        for x in range(0, w - block_size + 1, block_size):
            block = img[y:y+block_size, x:x+block_size]
            lsb = (block & 1).ravel()
            counts = np.bincount(lsb, minlength=2).astype(np.float64)
            p = counts / counts.sum()
            p = p[p > 0]
            if len(p) > 0:
                ent = float(-(p * np.log2(p)).sum())
                entropies.append(ent)
    
    if not entropies:
        return 0.0, 0.0
    
    return float(np.mean(entropies)), float(np.std(entropies))

def chi_square_attack(img):
    """
    Chi-square attack para detectar LSB steganography.
    Valor alto (>277) indica probable mensaje oculto.
    """
    pixels = img.ravel()
    chi_square = 0.0
    
    for k in range(128):
        count_2k = np.sum(pixels == 2*k)
        count_2k1 = np.sum(pixels == 2*k + 1)
        
        expected = (count_2k + count_2k1) / 2.0
        
        if expected > 0:
            chi_square += ((count_2k - expected)**2 + 
                          (count_2k1 - expected)**2) / expected
    
    return float(chi_square)

def histogram_difference(cover_img, stego_img):
    """
    Diferencia entre histogramas normalizada.
    """
    hist_cover = np.histogram(cover_img.ravel(), bins=256, range=(0, 256))[0]
    hist_stego = np.histogram(stego_img.ravel(), bins=256, range=(0, 256))[0]
    
    diff = np.abs(hist_cover - hist_stego)
    total_pixels = cover_img.size
    
    return float(np.sum(diff)) / total_pixels

def count_modified_bits(original, stego):
    """Cuenta bits distintos entre cover y stego."""
    if original.dtype != np.uint8: original = original.astype(np.uint8)
    if stego.dtype != np.uint8: stego = stego.astype(np.uint8)
    diff = np.bitwise_xor(original, stego)
    modified_bits = int(np.unpackbits(diff).sum())
    total_bits = diff.size * 8
    percent = (modified_bits / total_bits) * 100.0
    return modified_bits, percent

def count_modified_pixels(original, stego):
    """Cuenta píxeles modificados (al menos 1 bit diferente)."""
    diff = (original != stego)
    modified_pixels = int(np.sum(diff))
    total_pixels = original.size
    percent = (modified_pixels / total_pixels) * 100.0
    return modified_pixels, percent

def calculate_all_metrics(image_name, method, cover_img, stego_img):
    if cover_img.shape != stego_img.shape:
        raise ValueError(f"Dimensiones distintas para {image_name}/{method}")
    
    m = {}
    m["image"] = image_name
    m["variant"] = method
    
    # Calidad visual
    m["psnr"] = psnr(cover_img, stego_img)
    m["mse"] = mse(cover_img, stego_img)
    
    # Entropía general
    m["entropy_cover"] = shannon_entropy(cover_img)
    m["entropy_stego"] = shannon_entropy(stego_img)
    m["entropy_diff"] = m["entropy_stego"] - m["entropy_cover"]
    
    # Entropía LSB global
    m["lsb_entropy_cover"] = lsb_entropy(cover_img)
    m["lsb_entropy_stego"] = lsb_entropy(stego_img)
    m["lsb_entropy_diff"] = m["lsb_entropy_stego"] - m["lsb_entropy_cover"]
    
    # Entropía LSB por bloques
    lsb_blk_cov_mean, lsb_blk_cov_std = lsb_entropy_blocks(cover_img)
    lsb_blk_ste_mean, lsb_blk_ste_std = lsb_entropy_blocks(stego_img)
    m["lsb_block_entropy_cover_mean"] = lsb_blk_cov_mean
    m["lsb_block_entropy_stego_mean"] = lsb_blk_ste_mean
    m["lsb_block_entropy_diff"] = lsb_blk_ste_mean - lsb_blk_cov_mean
    
    # Chi-square attack
    m["chi_square_cover"] = chi_square_attack(cover_img)
    m["chi_square_stego"] = chi_square_attack(stego_img)
    m["chi_square_diff"] = m["chi_square_stego"] - m["chi_square_cover"]
    
    # Diferencia de histograma
    m["histogram_diff"] = histogram_difference(cover_img, stego_img)
    
    # Bits modificados
    bits, pct_bits = count_modified_bits(cover_img, stego_img)
    m["modified_bits"] = bits
    m["modified_bits_percent"] = pct_bits
    
    # Píxeles modificados
    pixels, pct_pixels = count_modified_pixels(cover_img, stego_img)
    m["modified_pixels"] = pixels
    m["modified_pixels_percent"] = pct_pixels
    
    return m

# ---------- ejecución ----------
def main():
    covers = {}
    for path in glob.glob(os.path.join(RESOURCES_DIR, "*.bmp")):
        base = os.path.splitext(os.path.basename(path))[0]
        covers[base] = path
    
    if not covers:
        raise RuntimeError(f"No encontré imágenes cover en {RESOURCES_DIR}/*.bmp")

    rows = []

    for base, cover_path in sorted(covers.items()):
        cover_img = load_image(cover_path)

        for method in VARIANTS:
            stego_path = os.path.join(OUT_DIR, f"{base}_{method}.bmp")
            if not os.path.exists(stego_path):
                print(f"[Aviso] Falta variante: {stego_path}")
                continue

            stego_img = load_image(stego_path)
            try:
                row = calculate_all_metrics(base, method, cover_img, stego_img)
                rows.append(row)
                print(f"[OK] {base}/{method}")
            except Exception as e:
                print(f"[Error] {base}/{method}: {e}")

    if not rows:
        raise RuntimeError("No se pudieron calcular métricas")

    df = pd.DataFrame(rows)
    
    os.makedirs(OUT_DIR, exist_ok=True)
    csv_path = os.path.join(OUT_DIR, "metrics_complete.csv")
    df.sort_values(["image","variant"]).to_csv(csv_path, index=False)

    # Mostrar resumen
    summary_cols = ["image", "variant", "psnr", "mse", "entropy_diff", 
                   "lsb_entropy_diff", "chi_square_diff", "modified_bits_percent"]
    
    print("\n=== RESUMEN DE MÉTRICAS ===")
    print(df[summary_cols].sort_values(["image","variant"]).to_string(index=False))
    print(f"\nCSV completo guardado: {csv_path}")

if __name__ == "__main__":
    main()