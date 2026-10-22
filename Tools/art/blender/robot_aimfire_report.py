# task-287-01: aim / fire checks from the SAVED half-float texture; exec'd by meshy_bake_anim.py (needs SQ, ST, qmat, SLAMP, SLAM_LIST, SLAMC, NRUN, NF, REST_V, FOOT, WARN, PARTS, att14_err)
_rows = []
_pos = {}
for _i, _r in enumerate(SLAM_LIST):
    _f = NRUN + _i
    _pos[_f] = {n: REST_V[n] @ qmat(SQ[_f, pi]).T + ST[_f, pi] for pi, n in enumerate(PARTS)}
    _tipc, _ext, _ax = SLAMP.tip(_pos[_f]["ForearmHand_R"])
    _slide = max(float(np.linalg.norm(_pos[_f][n] - REST_V[n], axis=1).max()) for n in FOOT)
    _body = max(float(np.linalg.norm(_pos[_f][n] - REST_V[n], axis=1).max()) for n in ("Pelvis", "HeadTorso", "Thigh_L", "Thigh_R", "UpperArm_L", "ForearmHand_L"))
    _rows.append(dict(texture_row=_f, clip=_r["clip"], frame_in_clip=_r["row_in_clip"], time_s=round(_r["row_in_clip"] / 30.0, 4), params={k: round(v, 4) for k, v in _r["params"].items()},
                      cannon_tip_robot_cm=[round(float(x), 2) for x in _tipc], cannon_tip_unreal_cm=[round(float(_tipc[0]), 2), round(float(-_tipc[1]), 2), round(float(_tipc[2]), 2)],
                      muzzle_farthest_vertex_robot_cm=[round(float(x), 2) for x in _ext],
                      cannon_axis_elevation_deg=round(math.degrees(math.asin(_ax[2])), 3), cannon_axis_azimuth_deg=round(math.degrees(math.atan2(_ax[1], _ax[0])), 3),
                      foot_max_displacement_from_rest_cm=round(_slide, 4), other_nonshooting_parts_max_displacement_cm=round(_body, 4)))
_na, _nh, _nf = SLAMP.n_aim, SLAMP.n_hold, SLAMP.n_fire
_r_aim_end, _r_aim0, _r_fire0, _r_fire_end = NRUN + _na - 1, NRUN, NRUN + _na + _nh, NF - 1
_ret = max(float(np.linalg.norm(_pos[_r_fire_end][n] - _pos[_r_aim_end][n], axis=1).max()) for n in PARTS)
_fire0 = max(float(np.linalg.norm(_pos[_r_fire0][n] - _pos[_r_aim_end][n], axis=1).max()) for n in PARTS)
_rest_rows = max(float(np.linalg.norm(_pos[f][n] - REST_V[n], axis=1).max()) for f in range(NRUN + _na, NRUN + _na + _nh) for n in PARTS)
_aim0 = max(float(np.linalg.norm(_pos[_r_aim0][n] - REST_V[n], axis=1).max()) for n in PARTS)
_peak = max(range(_nf), key=lambda k: SLAMP.recoil_w(k))
_rest_tip = SLAMP.tip(REST_V["ForearmHand_R"])
_tr = {r["frame_in_clip"]: r for r in _rows if r["clip"] == "aim"}
_te = _tr[_na - 1]
_hold_from = int(math.ceil(float(SLAMC["aim_frames"])))
SLAM_REP = dict(
    clips=dict(aim=dict(clip_id=1, rows="%d..%d" % (NRUN, _r_aim_end), frames=_na, behaviour="clamped (30 fps, frame = floor(t x 30)): arm raised with a smoothstep over frames 0..%d (reached at frame %d = %.3f s), held to row %d = aim pose" % (int(SLAMC["aim_frames"]), int(SLAMC["aim_frames"]), SLAMC["aim_frames"] / 30.0, _r_aim_end)),
               rest=dict(clip_id=2, rows="%d..%d" % (NRUN + _na, NRUN + _na + _nh - 1), frames=_nh, behaviour="LOOPED by the shader with RunRate: every row = exact rest pose (RestClipId 2 unchanged)"),
               fire=dict(clip_id=3, rows="%d..%d" % (_r_fire0, NF - 1), frames=_nf, behaviour="clamped: frame 0 = aim pose, recoil peak at frame %d (%.3f s), back to the aim pose at frame %d (%.3f s), held to the last row = aim pose" % (_peak, _peak / 30.0, int(SLAMC["return_frame"]), SLAMC["return_frame"] / 30.0))),
    method=dict(cannon_axis="first principal axis (+X sign) of ForearmHand_R vertices farther than %.2f of the elbow reach (%d vertices)" % (float(SLAMC.get("cannon_fraction", 0.35)), int(SLAMP.cannon.sum())), tip="centroid of the cannon vertices within %.1f cm of the farthest point along the axis (muzzle face centre); the farthest vertex is also listed" % float(SLAMC.get("tip_depth_cm", 3.0)), unreal_axes="Unreal = (X, -Y, Z) of the robot space, cm, floor origin (mesh z 0 = floor)"),
    keys=dict(aim_upper_arm_R_euler_deg=dict(y=round(SLAMP.u, 4), z=round(SLAMP.yaw, 4)), recoil=dict(shoulder_back_cm=SLAMC["shoulder_back_cm"], forearm_back_cm=SLAMC["forearm_back_cm"], forearm_pitch_deg=SLAMC["forearm_pitch_deg"], peak_frame=_peak, return_frame=SLAMC["return_frame"])),
    rest_pose=dict(cannon_tip_robot_cm=[round(float(x), 2) for x in _rest_tip[0]], cannon_axis_elevation_deg=round(math.degrees(math.asin(_rest_tip[2][2])), 3)),
    aim_end=dict(texture_row=_r_aim_end, time_to_reach_s=round(SLAMC["aim_frames"] / 30.0, 4), cannon_tip_robot_cm=_te["cannon_tip_robot_cm"], cannon_tip_unreal_cm=_te["cannon_tip_unreal_cm"], muzzle_farthest_vertex_robot_cm=_te["muzzle_farthest_vertex_robot_cm"],
                 cannon_axis_elevation_deg=_te["cannon_axis_elevation_deg"], cannon_axis_azimuth_deg_from_plus_x_horizontal=_te["cannon_axis_azimuth_deg"]),
    recoil=dict(peak_row=_r_fire0 + _peak, peak_cannon_tip_unreal_cm=_rows[_na + _nh + _peak]["cannon_tip_unreal_cm"], return_row=_r_fire0 + int(SLAMC["return_frame"]), fire_frame0_vs_aim_end_max_vertex_error_cm=round(_fire0, 5), last_fire_row_vs_aim_end_max_vertex_error_cm=round(_ret, 5),
                return_error_cm=round(max(_ret, float(np.linalg.norm(_pos[_r_fire0 + int(SLAMC["return_frame"])]["ForearmHand_R"] - _pos[_r_aim_end]["ForearmHand_R"], axis=1).max())), 5)),
    rest_rows_vs_source_max_vertex_error_cm=round(_rest_rows, 5), aim_row0_vs_source_max_vertex_error_cm=round(_aim0, 5),
    max_foot_displacement_cm=max(r["foot_max_displacement_from_rest_cm"] for r in _rows), max_non_shooting_part_displacement_cm=max(r["other_nonshooting_parts_max_displacement_cm"] for r in _rows),
    per_row=_rows)
if SLAM_REP["max_foot_displacement_cm"] > 0.1:
    WARN.append("feet move during the aim and fire clips by more than 0.1 cm")
