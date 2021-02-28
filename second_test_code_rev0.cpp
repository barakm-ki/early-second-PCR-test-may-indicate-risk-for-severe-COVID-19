	if (CORONA_FALSE_NEGATIVE == 1) {


		fprintf(stderr, "CORONA_ false negative analysis \n");

		string fn_dir = "D:\\projects\\corona_maccabi\\pattern\\";
		string symp_dir = "D:\\projects\\corona_maccabi\\symptoms\\";
		string cohort_fname = symp_dir + "corona_full_cohort_20200421.txt";
		string symp_fname = symp_dir + "corona_symptoms_orig3.csv";
		string tests = symp_dir + "tests_0805.txt";
		string diag_fname = symp_dir + "diagnosis_0805.txt";
		string symp_list_fname = symp_dir + "symp_list.txt";
		string summary_fname = symp_dir + "symp_summary.txt";
		string summary_or_fname = symp_dir + "symp_summary_or.txt";

		string karni_kod_mapping_fname = symp_dir + "karni_symp_mapping.txt";
		string karni_groups_fname = symp_dir + "karni_symp_mapping_groups.txt";
		string symp_all_medical_fname = symp_dir + "symp_all_medical.txt";
		string symp_id_fname = symp_dir + "maccabi_diag_from_medical_record_id.txt";


		//read tests
		map<int, vector<int>> map_id_vec_vals;
		map<int, vector<int>> map_id_vec_dates;
		map<int, int> test_neg_first;
		map<int, int> test_pos_first;
		map<int, int>  test_any;
		map<int, int> id_age;
		map<int, int> id_gender;

		map<int, int> test_first_ever;

		if (1 == 1) {
			int count = 0;
			string fname = tests;
			fprintf(stderr, "read %s \n", fname.c_str());
			ifstream inf(fname); if (!inf) { fprintf(stderr, "======Can't open file %s\n", fname.c_str()); return -1; }
			while (getline(inf, curr_line)) {
				if ((curr_line.size() > 0) && (curr_line[0] != '#')) {
					count++;
					if (count == 1) continue;
					split(fields, curr_line, boost::is_any_of("\t"));
					int id = stoi(fields[0]);
					int age = stoi(fields[5]);
					int gender = stoi(fields[6]);
					test_any[id] = 1;
					id_age[id] = age;
					id_gender[id] = gender;

					//fprintf(stderr, "%s\n", curr_line.c_str());
					//getchar();

					string SAMPLE_RESULT_DATE = fields[3];
					string execution_date = fields[4];
					int TEST_RESULT_CD = stoi(fields[1]);
					string final_date;
					if (execution_date == "#NULL!" && convert_date(SAMPLE_RESULT_DATE) == 20200101) continue;

					int new_date;
					if (execution_date != "#NULL!") {
						final_date = execution_date;
						new_date = convert_date(final_date);
					}
					else {
						final_date = SAMPLE_RESULT_DATE;
						new_date = convert_date(final_date);
					}
					//fprintf(stderr, "id %i result %i date %i \n", id, TEST_RESULT_CD, new_date);
					//getchar();

					if (TEST_RESULT_CD == 1 || TEST_RESULT_CD == 2) {
						if (test_first_ever.find(id) == test_first_ever.end())
							test_first_ever[id] = new_date;
						else if (new_date < test_first_ever[id]) test_first_ever[id] = new_date;
					}

					if (TEST_RESULT_CD == 1) {
						if (test_neg_first.find(id) == test_neg_first.end())
							test_neg_first[id] = new_date;
						else if (new_date < test_neg_first[id]) test_neg_first[id] = new_date;
					}

					if (TEST_RESULT_CD == 2) {
						if (test_pos_first.find(id) == test_pos_first.end())
							test_pos_first[id] = new_date;
						else if (new_date < test_pos_first[id]) test_pos_first[id] = new_date;
					}

					//if (new_date >= 20200415) {
						map_id_vec_vals[id].push_back(TEST_RESULT_CD - 1);
						map_id_vec_dates[id].push_back(new_date);
					//}

				}

			}
		}

		//read_diagnosis
		map<int, int> id_diagnosys_corona_date;
		if (1 == 1) {
			map<string, int> map_diagnosis_names;
			map<int, int>id_diagnosys_corona;

			int count = 0;
			string fname = diag_fname;
			fprintf(stderr, "read %s \n", fname.c_str());
			ifstream inf(fname); if (!inf) { fprintf(stderr, "======Can't open file %s\n", fname.c_str()); return -1; }
			while (getline(inf, curr_line)) {
				if ((curr_line.size() > 0) && (curr_line[0] != '#')) {
					count++;
					if (count == 1) {
						split(fields, curr_line, boost::is_any_of("\t"));
						for (int jj = 0; jj < fields.size(); jj++)
							map_diagnosis_names[fields[jj]] = jj;
						continue;
					}
					split(fields, curr_line, boost::is_any_of("\t"));
					int id = stoi(fields[map_diagnosis_names["RANDOM_ID"]]);
					int DIAGNOSIS_TYPE_CODE = stoi(fields[map_diagnosis_names["DIAGNOSIS_TYPE_CODE"]]);
					string VISIT_DATE = fields[map_diagnosis_names["VISIT_DATE"]];
					int VISIT_DATE_ki = convert_date(VISIT_DATE);

					if (DIAGNOSIS_TYPE_CODE == 3 && id_diagnosys_corona.find(id) == id_diagnosys_corona.end()) {
						id_diagnosys_corona[id] = 1;
						id_diagnosys_corona_date[id] = VISIT_DATE_ki;
					}
					else if (DIAGNOSIS_TYPE_CODE == 3 && id_diagnosys_corona.find(id) != id_diagnosys_corona.end()) {
						if (VISIT_DATE_ki < id_diagnosys_corona_date[id])  id_diagnosys_corona_date[id] = VISIT_DATE_ki;
					}
				}
			}
		}

		//pos final
		map<int, int> corona_pos_final;
		if (1 == 1) {
			for (auto it = test_pos_first.begin(); it != test_pos_first.end(); it++) {
				int id = it->first;
				int pos_date = it->second;
				corona_pos_final[id] = pos_date;
				if (id_diagnosys_corona_date.find(id) != id_diagnosys_corona_date.end()) {
					if (id_diagnosys_corona_date[id] < pos_date) corona_pos_final[id] = id_diagnosys_corona_date[id];
				}
			}
			for (auto it = id_diagnosys_corona_date.begin(); it != id_diagnosys_corona_date.end(); it++) {
				int id = it->first;
				int diag_date = it->second;
				if (corona_pos_final.find(id) == corona_pos_final.end()) corona_pos_final[id] = diag_date;

				if (test_any.find(id) == test_any.end()) test_any[id] = 1;
			}
		}


		//****** false negative analysis 
		if (1 == 1) {

			//old analysis
			if (1 == 2) {
				fprintf(stderr, "false negative analysis .....\n");
				//false negative analysis
				FILE *fout_partt = fopen("patten.txt", "w");
				fprintf(fout_partt, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "id", "diag_date", "diag_diff", "size", "date_0", "date_1", "date_2", "val_0", "val_1", "val_2");
				map<string, int> map_fpfn;
				map<string, map<int, map<int, int>>> map_pat_diff;
				map<string, map<int, int>> map_pat_days_from_diag;
				int diag_before_pos = 0;
				for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
					int id = it->first;
					int diag_date = -9;
					if (corona_pos_final.find(id) != corona_pos_final.end())  diag_date = corona_pos_final[id];

					if (it->second.size() < 3) continue;

					vector<int> temp_vec = it->second;
					int count = 0;
					for (int ii = 2; ii < temp_vec.size(); ii++) {

						int date_0 = temp_vec[count];
						int date_1 = temp_vec[count + 1];
						int date_2 = temp_vec[count + 2];

						int diff01 = datediff(date_0, date_1);
						int diff12 = datediff(date_1, date_2);
						if (diff01 <= 4 && diff12 <= 4) {
							int val_0 = map_id_vec_vals[id][count];
							int val_1 = map_id_vec_vals[id][count + 1];
							int val_2 = map_id_vec_vals[id][count + 2];

							int diag_diff = -9;
							if (diag_date != -9) diag_diff = datediff(diag_date, date_0);

							fprintf(fout_partt, "%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n", id, diag_date, diag_diff, (int)temp_vec.size(), date_0, date_1, date_2, val_0, val_1, val_2);

							string patt = to_string(val_0) + to_string(val_1) + to_string(val_2);
							map_fpfn[patt]++;
							map_pat_diff[patt][diff01][diff12]++;
							map_pat_days_from_diag[patt][diag_diff]++;
						}


						count++;
					}
				}
				fclose(fout_partt);

				int count_qa = 0;
				for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
					int id = it->first;
					vector<int> temp_vec = it->second;
					for (int ii = 1; ii < temp_vec.size(); ii++) {
						if (temp_vec[ii] < 20200101) {
							fprintf(stderr, "%i %i %i \n", id, temp_vec[ii], temp_vec[ii - 1]);
							count_qa++;
						}
						if (temp_vec[ii] < temp_vec[ii - 1]) {
							fprintf(stderr, "%i %i %i \n", id, temp_vec[ii], temp_vec[ii - 1]);
							count_qa++;
						}
					}
				}
				fprintf(stderr, "count_qa : %i\n", count_qa);
			}


			//******new patten

			string fname_out = fn_dir + "fn_2pos_at_the_end.txt";
			FILE *fout = fopen(fname_out.c_str(), "w");
			//fprintf(fout, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "Age", "Gender", "diff_from_diagnosis","pos","neg","running_sum_pos","running_sum_neg", "observed_fn", "avg_fn", "CI_95_lower", "CI_95_upper");
			fprintf(fout, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "param", "Age", "Gender", "diff_from_diagnosis", "pos", "neg", "All");

			int temp_win = 0;
			//fprintf(stderr, "diff = %i ", datediff(20200301, 20200501));
			//getchar();


			//fprintf(stderr, "%i\t%i\t%i\t%i\n", temp_win, temp_date, temp_date_from, temp_date_to);
			//getchar();

			//for (int temp_gender=0; temp_gender<=2; temp_gender++) {
			for (int temp_gender = 0; temp_gender <= 2; temp_gender++) {
				string gender_s;
				if (temp_gender == 0) gender_s = "Gender_All";
				else if (temp_gender == 1) gender_s = "Female";
				else if (temp_gender == 2) gender_s = "Male";


				//for (int temp_age_group = 0; temp_age_group <= 2; temp_age_group++) {
				for (int temp_age_group = 0; temp_age_group <= 0; temp_age_group++) {

					string age_s;
					if (temp_age_group == 0) age_s = "Age_All";
					else if (temp_age_group == 1) age_s = "Age_0_64";
					else if (temp_age_group == 2) age_s = "Age_65_100";


					for (int mm = 0; mm <= 0; mm++) {

						int temp_date = dateadd(20200301, mm);
						int temp_date_from = dateadd(temp_date, -1 * (temp_win / 2));
						int temp_date_to = dateadd(temp_date, 1 * (temp_win / 2));

					//fprintf(stderr, "%s %s \n", gender_s.c_str(), age_s.c_str());

						//date for id is the date of the first test 
						if (1 == 2) {

							int NBOOT = 1;

							vector<int> vec_ids;
							for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
								int id = it->first;
								//fprintf(stderr, "%i %i %i %i \n", id, test_first_ever[id], temp_date_from, temp_date_to);
								//getchar();
								if (test_first_ever[id] >= temp_date_from && test_first_ever[id] <= temp_date_to)
									vec_ids.push_back(id);
							}
							int n_ids = (int)vec_ids.size();
							//fprintf(stderr, "nids %i datefrom %i dateto %i \n", n_ids, temp_date_from, temp_date_to);
							//getchar();

							std::random_device rd;
							std::mt19937 gen(rd());
							std::default_random_engine generator(9999);
							std::uniform_int_distribution<> distribution(0, n_ids - 1);

							map<int, vector<float>> fn_vec_map;

							map<int, int> map_diff_pos_0;
							map<int, int> map_diff_neg_0;
							map<int, int> map_running_sum_pos_0;
							map<int, int> map_running_sum_neg_0;

							for (int temp_boot = 0; temp_boot < NBOOT; temp_boot++) {

								vector<int> boot_ids;
								if (temp_boot == 0) boot_ids = vec_ids;
								else {
									for (int ii = 0; ii < n_ids; ii++) {
										int idx = distribution(gen);
										boot_ids.push_back(vec_ids[idx]);
									}
								}

								map<int, map<int, int>> map_new_pattern;
								map<int, int> map_diff_pos;
								map<int, int> map_diff_neg;
								map<int, int> map_diff_all;
								for (int rr = 0; rr < boot_ids.size(); rr++) {
									//for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
									int id = boot_ids[rr];

									//if (id % 2 == 1) continue;
									//int id = it->first;

									if (temp_gender == 0) {}
									else if (temp_gender == 1) {
										if (id_gender[id] == 0) continue;
									}
									else if (temp_gender == 2) {
										if (id_gender[id] == 1) continue;
									}

									if (temp_age_group == 0) {}
									else if (temp_age_group == 1) {
										if (id_age[id] >= 65) continue;
									}
									else if (temp_age_group == 2) {
										if (id_age[id] < 65) continue;
									}
									//if (id_age[id] > 60) continue;
									//if (id_gender[id] == 0) continue;
									//vector<int> temp_vec = it->second;
									vector<int> temp_vec = map_id_vec_dates[id];
									int first_pos_i = -9;
									for (int ii = 0; ii < temp_vec.size(); ii++) {
										if (map_id_vec_vals[id][ii] == 1) {
											first_pos_i = ii;
											break;
										}
									}

									int count_pos = 0;
									for (int ii = 0; ii < temp_vec.size(); ii++) {
										if (map_id_vec_vals[id][ii] == 1) count_pos++;
									}

									//if (count_pos < 2) continue;
									if (count_pos < 3) continue;
									int count_temp_pos = 0;
									int pos_between = 0;
									int neg_between = 0;
									for (int ii = (int)temp_vec.size() - 1; ii > first_pos_i; ii--) {

										//if (count_temp_pos >= 1) {
										if (count_temp_pos >= 2) {
											if (map_id_vec_vals[id][ii] == 1) pos_between++;
											if (map_id_vec_vals[id][ii] == 0) neg_between++;

											int diff = datediff(map_id_vec_dates[id][first_pos_i], map_id_vec_dates[id][ii]);
											if (diff > 35) diff = 35;
											if (map_id_vec_vals[id][ii] == 1) {
												map_diff_pos[diff]++;
												if (temp_boot == 0) map_diff_pos_0[diff]++;
											}
											if (map_id_vec_vals[id][ii] == 0) {
												map_diff_neg[diff]++;
												if (temp_boot == 0) map_diff_neg_0[diff]++;
											}
											map_diff_all[diff]++;
										}

										if (map_id_vec_vals[id][ii] == 1) count_temp_pos++;
									}
									//fprintf(stderr, "%i\t%i\t%i\n", id, pos_between, neg_between);
									//getchar();

									map_new_pattern[pos_between][neg_between]++;
								}

								if (1 == 2) {

									fprintf(stderr, "new pattern ....\n");
									FILE *fout_new_pattenr = fopen("new_pattern_1pos_at_the_end.txt", "w");
									fprintf(fout_new_pattenr, "%s\t%s\t%s\n", "pos_between", "neg_between", "count");
									for (auto it = map_new_pattern.begin(); it != map_new_pattern.end(); it++) {
										int pos_between = it->first;
										for (auto it1 = map_new_pattern[pos_between].begin(); it1 != map_new_pattern[pos_between].end(); it1++) {
											int neg_between = it1->first;
											int count = it1->second;
											fprintf(fout_new_pattenr, "%i\t%i\t%i\n", pos_between, neg_between, count);
										}
									}
									fclose(fout_new_pattenr);
								}

								//fprintf(stderr, "==== diff analysis ....\n");
								//FILE *fout_win = fopen("tests_in_win.txt", "w");
								int running_sum_neg = 0, running_sum_pos = 0;
								float fn_accum = 0;
								for (auto it = map_diff_all.begin(); it != map_diff_all.end(); it++) {
									int diff = it->first;
									running_sum_neg += map_diff_neg[diff];
									running_sum_pos += map_diff_pos[diff];

									//fprintf(stderr, "%i\t%i\t%i\t%i\t%i\t%f\t%f\n", diff, map_diff_pos[diff], map_diff_neg[diff], running_sum_pos, running_sum_neg,
										//(float)map_diff_neg[diff] / ((float)map_diff_pos[diff] + (float)map_diff_neg[diff]),
										//(float)running_sum_neg / ((float)running_sum_pos + (float)running_sum_neg)
									//);
									//getchar();

									fn_accum = (float)running_sum_neg / ((float)running_sum_pos + (float)running_sum_neg);
									fn_vec_map[diff].push_back(fn_accum);

									if (temp_boot == 0) {
										map_running_sum_pos_0[diff] = running_sum_pos;
										map_running_sum_neg_0[diff] = running_sum_neg;
									}

								}

								//fprintf(stderr, "%i fn_accum  %f\n",temp_boot, fn_accum);
								//fn_vec_map[999].push_back(fn_accum);

							}

							for (auto it = fn_vec_map.begin(); it != fn_vec_map.end(); it++) {
								int diff = it->first;
								vector<float> prc_vals = { (float) 0.05,(float) 0.95 };
								vector<float> prs_out;
								get_prctils(fn_vec_map[diff], prc_vals, prs_out);

								float avg_fn = 0;
								for (int jj = 0; jj < fn_vec_map[diff].size(); jj++)
									avg_fn += fn_vec_map[diff][jj];
								avg_fn /= (int)fn_vec_map[diff].size();

								//if (diff == 10) {
									//fprintf(fout, "%s\t%s\t%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\n", age_s.c_str(), gender_s.c_str(), diff, map_diff_pos_0[diff], map_diff_neg_0[diff], map_running_sum_pos_0[diff], map_running_sum_neg_0[diff], fn_vec_map[diff][0], avg_fn, prs_out[0], prs_out[1]);
									fprintf(fout, "%i\t%s\t%s\t%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\n", temp_date, age_s.c_str(), gender_s.c_str(), diff, map_diff_pos_0[diff], map_diff_neg_0[diff], map_running_sum_pos_0[diff], map_running_sum_neg_0[diff], fn_vec_map[diff][0], avg_fn, prs_out[0], prs_out[1]);
									//fprintf(stderr, "%i\t%s\t%s\t%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\n", temp_date, age_s.c_str(), gender_s.c_str(), diff, map_diff_pos_0[diff], map_diff_neg_0[diff], map_running_sum_pos_0[diff], map_running_sum_neg_0[diff], fn_vec_map[diff][0], avg_fn, prs_out[0], prs_out[1]);
									//getchar();
									//fprintf(stderr, "%i\t%s\t%s\t%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\n", temp_date, age_s.c_str(), gender_s.c_str(), diff, map_diff_pos_0[diff], map_diff_neg_0[diff], map_running_sum_pos_0[diff], map_running_sum_neg_0[diff], fn_vec_map[diff][0], avg_fn, prs_out[0], prs_out[1]);
								//}
							}
						}
				
						//date for the date of the test
						if (1 == 1) {

							int NBOOT = 1;

							vector<int> vec_ids;
							for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
								int id = it->first;
								//fprintf(stderr, "%i %i %i %i \n", id, test_first_ever[id], temp_date_from, temp_date_to);
								//getchar();
								//if (test_first_ever[id] >= temp_date_from && test_first_ever[id] <= temp_date_to)
								vec_ids.push_back(id);
							}
							int n_ids = (int)vec_ids.size();
							//fprintf(stderr, "nids %i datefrom %i dateto %i \n", n_ids, temp_date_from, temp_date_to);
							//getchar();

							std::random_device rd;
							std::mt19937 gen(rd());
							std::default_random_engine generator(9999);
							std::uniform_int_distribution<> distribution(0, n_ids - 1);

							map<int, vector<float>> fn_vec_map;

							map<int, map<int, int>> map_diff_all_0;
							map<int,map<int, int>> map_diff_pos_0;
							map<int,map<int, int>> map_diff_neg_0;

							map<int, map<int, int>> map_diff_all_0_male;
							map<int, map<int, int>> map_diff_pos_0_male;
							map<int, map<int, int>> map_diff_neg_0_male;

							map<int, map<int, int>> map_diff_all_0_female;
							map<int, map<int, int>> map_diff_pos_0_female;
							map<int, map<int, int>> map_diff_neg_0_female;


							map<int, int> map_running_sum_pos_0;
							map<int, int> map_running_sum_neg_0;

							for (int temp_boot = 0; temp_boot < NBOOT; temp_boot++) {

								vector<int> boot_ids;
								if (temp_boot == 0) boot_ids = vec_ids;
								else {
									for (int ii = 0; ii < n_ids; ii++) {
										int idx = distribution(gen);
										boot_ids.push_back(vec_ids[idx]);
									}
								}

								map<int, map<int, int>> map_new_pattern;
								map<int, int> map_diff_pos;
								map<int, int> map_diff_neg;
								map<int, int> map_diff_all;
								for (int rr = 0; rr < boot_ids.size(); rr++) {
									//for (auto it = map_id_vec_dates.begin(); it != map_id_vec_dates.end(); it++) {
									int id = boot_ids[rr];

									//if (id % 2 == 1) continue;
									//int id = it->first;

									if (temp_gender == 0) {}
									else if (temp_gender == 1) {
										if (id_gender[id] == 0) continue;
									}
									else if (temp_gender == 2) {
										if (id_gender[id] == 1) continue;
									}

									if (temp_age_group == 0) {}
									else if (temp_age_group == 1) {
										if (id_age[id] >= 65) continue;
									}
									else if (temp_age_group == 2) {
										if (id_age[id] < 65) continue;
									}

									vector<int> temp_vec = map_id_vec_dates[id];
									int first_pos_i = -9;
									for (int ii = 0; ii < temp_vec.size(); ii++) {
										if (map_id_vec_vals[id][ii] == 1) {
											first_pos_i = ii;
											break;
										}
									}

									int count_pos = 0;
									for (int ii = 0; ii < temp_vec.size(); ii++) {
										if (map_id_vec_vals[id][ii] == 1) count_pos++;
									}

									//if (count_pos < 2) continue;
									if (count_pos < 3) continue;
									int count_temp_pos = 0;
									int pos_between = 0;
									int neg_between = 0;
									for (int ii = (int)temp_vec.size() - 1; ii > first_pos_i; ii--) {

										//if (count_temp_pos >= 1) {
										if (count_temp_pos >= 2) {
											if (map_id_vec_vals[id][ii] == 1) pos_between++;
											if (map_id_vec_vals[id][ii] == 0) neg_between++;

											int diff = datediff(map_id_vec_dates[id][first_pos_i], map_id_vec_dates[id][ii]);

											//int param = map_id_vec_dates[id][ii];
											int param = id_age[id];

											if (diff > 35) diff = 35;
											if (map_id_vec_vals[id][ii] == 1) {
												map_diff_pos[diff]++;
												if (temp_boot == 0) map_diff_pos_0[param][diff]++;
											}
											if (map_id_vec_vals[id][ii] == 0) {
												map_diff_neg[diff]++;
												if (temp_boot == 0) map_diff_neg_0[param][diff]++;
											}

											map_diff_all_0[param][diff]++;

											map_diff_all[diff]++;
										}

										if (map_id_vec_vals[id][ii] == 1) count_temp_pos++;
									}
									//fprintf(stderr, "%i\t%i\t%i\n", id, pos_between, neg_between);
									//getchar();

									map_new_pattern[pos_between][neg_between]++;
								}

							}

							for (auto it = map_diff_all_0.begin(); it != map_diff_all_0.end(); it++) {
								int date = it->first;

								for (auto it1 = map_diff_all_0[date].begin(); it1 != map_diff_all_0[date].end(); it1++) {
									int diff = it1->first;
									fprintf(fout, "%i\t%s\t%s\t%i\t%i\t%i\t%i\n", date, age_s.c_str(), gender_s.c_str(), diff, map_diff_pos_0[date][diff], map_diff_neg_0[date][diff], map_diff_all_0[date][diff]);
								}
							}
						}




					}
			}

			}
			fclose(fout);


			return 1;



		}


	}