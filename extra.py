import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

# Configuración de estilo
plt.style.use('seaborn-v0_8-paper')
sns.set_palette("husl")

# ============================================================================
# DATOS
# ============================================================================

images = ['Lenna', 'Peppers', 'Airplane']
methods = ['LSB1', 'LSB4', 'LSBI']
colors = {'LSB1': '#2E86AB', 'LSB4': '#A23B72', 'LSBI': '#F18F01'}

# PSNR 1KB
psnr_1kb = {
    'LSB1': [70.95, 70.87, 70.92],
    'LSB4': [58.50, 58.02, 57.62],
    'LSBI': [65.95, 60.33, 57.16]
}

# PSNR 90KB
psnr_90kb = {
    'LSB1': [51.58, 51.54, 51.53],
    'LSB4': [38.96, 38.58, 38.46],
    'LSBI': [51.41, 51.29, 50.42]
}

# MSE 1KB
mse_1kb = {
    'LSB1': [0.0052, 0.0053, 0.0053],
    'LSB4': [0.092, 0.10, 0.11],
    'LSBI': [0.017, 0.060, 0.12]
}

# MSE 90KB
mse_90kb = {
    'LSB1': [0.45, 0.46, 0.46],
    'LSB4': [8.26, 9.02, 9.26],
    'LSBI': [0.47, 0.48, 0.59]
}

# Entropía ΔH 90KB
entropy_90kb = {
    'LSB1': [0.99, 0.025, -0.00082],
    'LSB4': [0.83, 0.0049, 0.0028],
    'LSBI': [0.98, 0.025, -0.00037]
}

# ============================================================================
# GRÁFICO 1: BARRAS AGRUPADAS - PSNR (1KB vs 90KB)
# ============================================================================

# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# x = np.arange(len(images))
# width = 0.25

# # 1 KB
# for i, method in enumerate(methods):
#     ax1.bar(x + i*width, psnr_1kb[method], width, label=method, color=colors[method], alpha=0.8)

# ax1.set_ylabel('PSNR (dB)', fontsize=12)
# ax1.set_title('Mensaje 1 KB', fontsize=14, fontweight='bold')
# ax1.set_xticks(x + width)
# ax1.set_xticklabels(images)
# ax1.legend()
# ax1.grid(axis='y', alpha=0.3)
# ax1.axhline(y=40, color='red', linestyle='--', linewidth=1, alpha=0.5, label='Umbral perceptibilidad')

# # 90 KB
# for i, method in enumerate(methods):
#     ax2.bar(x + i*width, psnr_90kb[method], width, label=method, color=colors[method], alpha=0.8)

# ax2.set_ylabel('PSNR (dB)', fontsize=12)
# ax2.set_title('Mensaje 90 KB', fontsize=14, fontweight='bold')
# ax2.set_xticks(x + width)
# ax2.set_xticklabels(images)
# ax2.legend()
# ax2.grid(axis='y', alpha=0.3)
# ax2.axhline(y=40, color='red', linestyle='--', linewidth=1, alpha=0.5, label='Umbral perceptibilidad')

# plt.tight_layout()
# plt.savefig('01_psnr_comparison.png', dpi=300, bbox_inches='tight')
# print("✓ Guardado: 01_psnr_comparison.png")
# plt.show()

# ============================================================================
# GRÁFICO 3: BARRAS - MSE (escala logarítmica)
# ============================================================================

# fig, ax = plt.subplots(figsize=(10, 6))

# x = np.arange(len(methods))
# width = 0.35

# # Promedios
# mse_1kb_avg = [np.mean(mse_1kb[m]) for m in methods]
# mse_90kb_avg = [np.mean(mse_90kb[m]) for m in methods]

# bars1 = ax.bar(x - width/2, mse_1kb_avg, width, label='1 KB', alpha=0.8)
# bars2 = ax.bar(x + width/2, mse_90kb_avg, width, label='90 KB', alpha=0.8)

# ax.set_ylabel('MSE (escala logarítmica)', fontsize=12)
# ax.set_xlabel('Método', fontsize=12)
# ax.set_title('Comparación de MSE por método y tamaño de mensaje', fontsize=14, fontweight='bold')
# ax.set_xticks(x)
# ax.set_xticklabels(methods)
# ax.set_yscale('log')
# ax.legend()
# ax.grid(axis='y', alpha=0.3, which='both')

# # Anotaciones
# for i, (bar1, bar2) in enumerate(zip(bars1, bars2)):
#     height1 = bar1.get_height()
#     height2 = bar2.get_height()
#     ax.text(bar1.get_x() + bar1.get_width()/2., height1, f'{height1:.3f}',
#             ha='center', va='bottom', fontsize=9)
#     ax.text(bar2.get_x() + bar2.get_width()/2., height2, f'{height2:.2f}',
#             ha='center', va='bottom', fontsize=9)

# plt.tight_layout()
# plt.savefig('03_mse_comparison.png', dpi=300, bbox_inches='tight')
# print("✓ Guardado: 03_mse_comparison.png")
# plt.show()

# ============================================================================
# GRÁFICO 4: HEATMAP - Entropía ΔH (90KB)
# ============================================================================

# fig, ax = plt.subplots(figsize=(8, 5))

# # Crear matriz
# entropy_matrix = np.array([
#     [entropy_90kb['LSB1'][0], entropy_90kb['LSB4'][0], entropy_90kb['LSBI'][0]],  # Lenna
#     [entropy_90kb['LSB1'][1], entropy_90kb['LSB4'][1], entropy_90kb['LSBI'][1]],  # Peppers
#     [entropy_90kb['LSB1'][2], entropy_90kb['LSB4'][2], entropy_90kb['LSBI'][2]]   # Airplane
# ])

# # Heatmap
# im = ax.imshow(entropy_matrix, cmap='RdYlGn_r', aspect='auto', vmin=-0.1, vmax=1.0)

# # Configuración
# ax.set_xticks(np.arange(len(methods)))
# ax.set_yticks(np.arange(len(images)))
# ax.set_xticklabels(methods)
# ax.set_yticklabels(images)
# ax.set_title('Cambio de entropía (ΔH) con mensaje de 90 KB', fontsize=14, fontweight='bold')

# # Anotaciones con valores
# for i in range(len(images)):
#     for j in range(len(methods)):
#         value = entropy_matrix[i, j]
#         text = ax.text(j, i, f'{value:.3f}' if abs(value) > 0.01 else f'{value:.2e}',
#                       ha="center", va="center", color="black", fontsize=10, fontweight='bold')

# # Colorbar
# cbar = plt.colorbar(im, ax=ax)
# cbar.set_label('ΔH (bits)', rotation=270, labelpad=20)

# plt.tight_layout()
# plt.savefig('04_entropy_heatmap.png', dpi=300, bbox_inches='tight')
# print("✓ Guardado: 04_entropy_heatmap.png")
# plt.show()

# ============================================================================
# GRÁFICO 5: DISPERSIÓN - PSNR vs MSE
# ============================================================================

fig, ax = plt.subplots(figsize=(10, 7))

# Preparar datos
for method in methods:
    # 1 KB (círculos pequeños)
    psnr_1 = psnr_1kb[method]
    mse_1 = mse_1kb[method]
    ax.scatter(mse_1, psnr_1, s=100, alpha=0.6, color=colors[method], 
               marker='o', edgecolors='black', linewidth=1, label=f'{method} (1 KB)')
    
    # 90 KB (círculos grandes)
    psnr_90 = psnr_90kb[method]
    mse_90 = mse_90kb[method]
    ax.scatter(mse_90, psnr_90, s=300, alpha=0.6, color=colors[method], 
               marker='o', edgecolors='black', linewidth=2, label=f'{method} (90 KB)')

ax.set_xlabel('MSE (escala logarítmica)', fontsize=12)
ax.set_ylabel('PSNR (dB)', fontsize=12)
ax.set_title('Relación entre PSNR y MSE', fontsize=14, fontweight='bold')
ax.set_xscale('log')
ax.legend(fontsize=9, loc='best')
ax.grid(True, alpha=0.3, which='both')
ax.axhline(y=40, color='red', linestyle='--', linewidth=1, alpha=0.5)

plt.tight_layout()
plt.savefig('05_psnr_vs_mse.png', dpi=300, bbox_inches='tight')
print("✓ Guardado: 05_psnr_vs_mse.png")
plt.show()

# ============================================================================
# GRÁFICO 6: BARRAS APILADAS - % Bits modificados
# ============================================================================

fig, ax = plt.subplots(figsize=(10, 6))

bits_modified_90kb = [5.64, 6.07, 5.70]  # Promedio LSB1, LSB4, LSBI
bits_unmodified = [100 - x for x in bits_modified_90kb]

x = np.arange(len(methods))
width = 0.6

p1 = ax.bar(x, bits_modified_90kb, width, label='Bits modificados', alpha=0.8)
p2 = ax.bar(x, bits_unmodified, width, bottom=bits_modified_90kb, 
            label='Bits sin modificar', alpha=0.5)

ax.set_ylabel('Porcentaje de la imagen (%)', fontsize=12)
ax.set_xlabel('Método', fontsize=12)
ax.set_title('Distribución de bits modificados (90 KB)', fontsize=14, fontweight='bold')
ax.set_xticks(x)
ax.set_xticklabels(methods)
ax.legend()
ax.grid(axis='y', alpha=0.3)

# Anotaciones
for i, (mod, unmod) in enumerate(zip(bits_modified_90kb, bits_unmodified)):
    ax.text(i, mod/2, f'{mod:.1f}%', ha='center', va='center', 
            fontweight='bold', fontsize=11, color='white')

plt.tight_layout()
plt.savefig('06_bits_modified_distribution.png', dpi=300, bbox_inches='tight')
print("✓ Guardado: 06_bits_modified_distribution.png")
plt.show()

# ============================================================================
# GRÁFICO 7: COMPARACIÓN MÚLTIPLE (panel 2x2)
# ============================================================================

fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))

# Panel 1: PSNR 90KB
x = np.arange(len(images))
width = 0.25
for i, method in enumerate(methods):
    ax1.bar(x + i*width, psnr_90kb[method], width, label=method, color=colors[method], alpha=0.8)
ax1.set_ylabel('PSNR (dB)')
ax1.set_title('PSNR - 90 KB', fontweight='bold')
ax1.set_xticks(x + width)
ax1.set_xticklabels(images)
ax1.legend()
ax1.grid(axis='y', alpha=0.3)
ax1.axhline(y=40, color='red', linestyle='--', linewidth=1, alpha=0.5)

# Panel 2: MSE 90KB
mse_90kb_avg = [np.mean(mse_90kb[m]) for m in methods]
x = np.arange(len(methods))
bars = ax2.bar(x, mse_90kb_avg, color=[colors[m] for m in methods], alpha=0.8)
ax2.set_ylabel('MSE')
ax2.set_title('MSE promedio - 90 KB', fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(methods)
ax2.grid(axis='y', alpha=0.3)
for i, bar in enumerate(bars):
    height = bar.get_height()
    ax2.text(bar.get_x() + bar.get_width()/2., height, f'{height:.2f}',
            ha='center', va='bottom', fontsize=10)

# Panel 3: Entropía ΔH
entropy_avg = {m: np.mean(entropy_90kb[m]) for m in methods}
x = np.arange(len(methods))
bars = ax3.bar(x, [entropy_avg[m] for m in methods], 
               color=[colors[m] for m in methods], alpha=0.8)
ax3.set_ylabel('ΔH (bits)')
ax3.set_title('Cambio de entropía promedio - 90 KB', fontweight='bold')
ax3.set_xticks(x)
ax3.set_xticklabels(methods)
ax3.grid(axis='y', alpha=0.3)
for i, bar in enumerate(bars):
    height = bar.get_height()
    ax3.text(bar.get_x() + bar.get_width()/2., height, f'{height:.3f}',
            ha='center', va='bottom', fontsize=10)

# Panel 4: Escalamiento
sizes = ['1 KB', '90 KB']
x_pos = [0, 1]
for method in methods:
    avg_1kb = np.mean(psnr_1kb[method])
    avg_90kb = np.mean(psnr_90kb[method])
    ax4.plot(x_pos, [avg_1kb, avg_90kb], marker='o', markersize=10, 
            linewidth=2.5, label=method, color=colors[method])
ax4.set_ylabel('PSNR (dB)')
ax4.set_title('Escalamiento PSNR', fontweight='bold')
ax4.set_xticks(x_pos)
ax4.set_xticklabels(sizes)
ax4.legend()
ax4.grid(True, alpha=0.3)
ax4.axhline(y=40, color='red', linestyle='--', linewidth=1, alpha=0.5)

plt.suptitle('Resumen comparativo de métodos de esteganografía', 
             fontsize=16, fontweight='bold', y=1.00)
plt.tight_layout()
plt.savefig('07_comparison_panel.png', dpi=300, bbox_inches='tight')
print("✓ Guardado: 07_comparison_panel.png")
plt.show()

print("\n" + "="*60)
print("✅ TODOS LOS GRÁFICOS GENERADOS EXITOSAMENTE")
print("="*60)
print("\nArchivos guardados:")
print("  1. 01_psnr_comparison.png - Barras agrupadas PSNR")
print("  2. 02_psnr_scaling.png - Líneas escalamiento")
print("  3. 03_mse_comparison.png - Barras MSE")
print("  4. 04_entropy_heatmap.png - Heatmap entropía")
print("  5. 05_psnr_vs_mse.png - Dispersión PSNR vs MSE")
print("  6. 06_bits_modified_distribution.png - Barras apiladas")
print("  7. 07_comparison_panel.png - Panel comparativo 2×2")