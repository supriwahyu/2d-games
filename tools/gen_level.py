#!/usr/bin/env python3
"""Generate scenes/Main.tscn for our Mario-style Godot 4 + C GDExtension platformer."""
import os

OUT_PATH = os.path.join(os.path.dirname(os.path.dirname(__file__)), "scenes", "Main.tscn")

# Tile grid: 32px per unit. Ground top is at y = 496 (center y = 512).
# Level width: 90 tiles = 2880px.

lines = []
lines.append('[gd_scene load_steps=16 format=3]')
lines.append('')
lines.append('[ext_resource type="PackedScene" path="res://scenes/Player.tscn" id="1"]')
lines.append('[ext_resource type="PackedScene" path="res://scenes/Enemy.tscn" id="2"]')
lines.append('[ext_resource type="PackedScene" path="res://scenes/Coin.tscn" id="3"]')
lines.append('[ext_resource type="PackedScene" path="res://scenes/QuestionBlock.tscn" id="4"]')
lines.append('[ext_resource type="PackedScene" path="res://scenes/Flagpole.tscn" id="5"]')
lines.append('[ext_resource type="PackedScene" path="res://scenes/HUD.tscn" id="6"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/block_ground.png" id="7"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/block_brick.png" id="8"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/pipe.png" id="9"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/castle.png" id="10"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/cloud.png" id="11"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/hill.png" id="12"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/bush.png" id="13"]')
lines.append('[ext_resource type="Texture2D" path="res://assets/flowers.png" id="14"]')
lines.append('')
lines.append('[sub_resource type="RectangleShape2D" id="RectangleShape2D_1"]')
lines.append('size = Vector2(32, 32)')
lines.append('')
lines.append('[sub_resource type="RectangleShape2D" id="RectangleShape2D_2"]')
lines.append('size = Vector2(48, 64)')
lines.append('')
lines.append('[node name="Main" type="Node2D"]')
lines.append('')
lines.append('[node name="Scenery" type="Node2D" parent="."]')
lines.append('')

# Rolling hills
hills = [160, 680, 1180, 1620, 2020, 2440]
for idx, hx in enumerate(hills):
    lines.append(f'[node name="Hill{idx}" type="Sprite2D" parent="Scenery"]')
    lines.append(f'position = Vector2({hx}, 464)')
    lines.append('texture = ExtResource("12")')
    lines.append('')

# Bushes
bushes = [280, 520, 1020, 1460, 1900, 2320]
for idx, bx in enumerate(bushes):
    lines.append(f'[node name="Bush{idx}" type="Sprite2D" parent="Scenery"]')
    lines.append(f'position = Vector2({bx}, 480)')
    lines.append('texture = ExtResource("13")')
    lines.append('')

# Colorful flower patches
flowers = [200, 410, 720, 1110, 1300, 1550, 1960, 2220]
for idx, fx in enumerate(flowers):
    lines.append(f'[node name="Flowers{idx}" type="Sprite2D" parent="Scenery"]')
    lines.append(f'position = Vector2({fx}, 488)')
    lines.append('texture = ExtResource("14")')
    lines.append('')

# Clouds
clouds = [(140, 120), (420, 90), (760, 140), (1120, 100), (1500, 130), (1880, 95), (2260, 125), (2620, 105)]
for idx, (cx, cy) in enumerate(clouds):
    lines.append(f'[node name="Cloud{idx}" type="Sprite2D" parent="Scenery"]')
    lines.append(f'position = Vector2({cx}, {cy})')
    lines.append('texture = ExtResource("11")')
    lines.append('')

# Castle at the end
lines.append('[node name="Castle" type="Sprite2D" parent="Scenery"]')
lines.append('position = Vector2(2720, 448)')
lines.append('texture = ExtResource("10")')
lines.append('')

lines.append('[node name="Level" type="Node2D" parent="."]')
lines.append('')


def add_block(name, tex_id, x, y):
    lines.append(f'[node name="{name}" type="StaticBody2D" parent="Level"]')
    lines.append(f'position = Vector2({x}, {y})')
    lines.append(f'[node name="Sprite2D" type="Sprite2D" parent="Level/{name}"]')
    lines.append(f'texture = ExtResource("{tex_id}")')
    lines.append(f'[node name="Col" type="CollisionShape2D" parent="Level/{name}"]')
    lines.append('shape = SubResource("RectangleShape2D_1")')
    lines.append('')


# Ground with two Mario-style pits: tiles 24..26 and tiles 54..56
pits = {24, 25, 26, 54, 55, 56}
for tx in range(90):
    if tx in pits:
        continue
    for row, ty in enumerate([512, 544, 576]):
        add_block(f"G_{tx}_{row}", 7, tx * 32 + 16, ty)

# Brick platforms
bricks = [
    (11, 384), (13, 384), (15, 384),
    (31, 384), (32, 384), (34, 384),
    (36, 288), (37, 288), (38, 288), (39, 288),
    (46, 384), (47, 384),
    (61, 384), (63, 384), (64, 384),
    # Staircase before flagpole
    (71, 480),
    (72, 480), (72, 448),
    (73, 480), (73, 448), (73, 416),
    (74, 480), (74, 448), (74, 416), (74, 384),
]
for idx, (bx, by) in enumerate(bricks):
    add_block(f"Brick_{idx}", 8, bx * 32 + 16, by)

# Pipes
pipes = [(19 * 32, 464), (29 * 32, 464), (43 * 32, 464), (67 * 32, 464)]
for idx, (px, py) in enumerate(pipes):
    lines.append(f'[node name="Pipe_{idx}" type="StaticBody2D" parent="Level"]')
    lines.append(f'position = Vector2({px}, {py})')
    lines.append(f'[node name="Sprite2D" type="Sprite2D" parent="Level/Pipe_{idx}"]')
    lines.append('texture = ExtResource("9")')
    lines.append(f'[node name="Col" type="CollisionShape2D" parent="Level/Pipe_{idx}"]')
    lines.append('shape = SubResource("RectangleShape2D_2")')
    lines.append('')

# Question blocks
qblocks = [
    (9 * 32 + 16, 384),
    (12 * 32 + 16, 384),
    (14 * 32 + 16, 384),
    (33 * 32 + 16, 384),
    (37 * 32 + 16, 192),
    (62 * 32 + 16, 384),
]
for idx, (qx, qy) in enumerate(qblocks):
    lines.append(f'[node name="QBlock_{idx}" parent="Level" instance=ExtResource("4")]')
    lines.append(f'position = Vector2({qx}, {qy})')
    lines.append('')

# Coins
coins = [
    (9 * 32 + 16, 336),
    (12 * 32 + 16, 336),
    (14 * 32 + 16, 336),
    (25 * 32 + 16, 400),  # over first pit
    (36 * 32 + 16, 240), (37 * 32 + 16, 240), (38 * 32 + 16, 240), (39 * 32 + 16, 240),
    (55 * 32 + 16, 400),  # over second pit
    (61 * 32 + 16, 336), (62 * 32 + 16, 336), (63 * 32 + 16, 336),
]
for idx, (cx, cy) in enumerate(coins):
    lines.append(f'[node name="Coin_{idx}" parent="Level" instance=ExtResource("3")]')
    lines.append(f'position = Vector2({cx}, {cy})')
    lines.append('')

# Enemies (Goombas)
enemies = [
    (460, 470),
    (740, 470),
    (1080, 470),
    (1200, 240),  # on upper brick platform
    (1520, 470),
    (1980, 470),
    (2120, 470),
]
for idx, (ex, ey) in enumerate(enemies):
    lines.append(f'[node name="Enemy_{idx}" parent="Level" instance=ExtResource("2")]')
    lines.append(f'position = Vector2({ex}, {ey})')
    lines.append('')

# Flagpole
lines.append('[node name="Flagpole" parent="Level" instance=ExtResource("5")]')
lines.append('position = Vector2(2560, 416)')
lines.append('')

# Player
lines.append('[node name="Player" parent="." instance=ExtResource("1")]')
lines.append('position = Vector2(96, 464)')
lines.append('')

# HUD
lines.append('[node name="HUD" parent="." instance=ExtResource("6")]')
lines.append('')

with open(OUT_PATH, "w") as f:
    f.write("\n".join(lines))

print("Wrote", OUT_PATH)
