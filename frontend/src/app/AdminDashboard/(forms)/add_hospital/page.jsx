'use client';

import { useRouter, useSearchParams } from 'next/navigation';
import { useState, useEffect } from 'react';

export default function AddHospitalForm() {
  const router = useRouter();
  const params = useSearchParams();

  const [form, setForm] = useState({
    name: '',
    area: '',
    beds: 0,
    specialization: '',
    rating: '',
  });
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    // Prefill values if provided in query
    setForm({
      name: params.get('name') || '',
      area: params.get('area') || '',
      beds: parseInt(params.get('beds')) || 0,
      specialization: params.get('specialization') || '',
      rating: parseInt(params.get('rating')) || '',
    });
  }, [params]);

  const handleChange = (e) => {
    const { id, value } = e.target;
    setForm((prev) => ({
      ...prev,
      [id.replace('hospital-', '')]: id === 'hospital-beds' || id === 'hospital-rating' ? parseInt(value) : value,
    }));
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);

    try {
      const res = await fetch('/api/mongo?collection=Hospital', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ data: form }),
      });

      const result = await res.json();

      if (res.ok) {
        alert('✅ Hospital saved successfully!');
        router.push('/AdminDashboard');
      } else {
        alert('❌ Failed to save: ' + (result?.error || 'Unknown error'));
      }
    } catch (err) {
      console.error('Submit error:', err);
      alert('❌ Network/server error');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="container py-5">
      <div className="row justify-content-center">
        <div className="col-md-8">
          <div className="card shadow rounded-4">
            <div className="card-body p-4">
              <h4 className="fw-bold mb-4 text-primary">
                {params.get('name') ? 'Edit Hospital' : 'Add New Hospital'}
              </h4>

              <form onSubmit={handleSubmit}>
                <div className="row g-3">
                  <div className="col-md-6">
                    <div className="form-floating">
                      <input
                        type="text"
                        className="form-control"
                        id="hospital-name"
                        value={form.name}
                        onChange={handleChange}
                        required
                      />
                      <label htmlFor="hospital-name">Hospital Name *</label>
                    </div>
                  </div>

                  <div className="col-md-6">
                    <div className="form-floating">
                      <input
                        type="text"
                        className="form-control"
                        id="hospital-area"
                        value={form.area}
                        onChange={handleChange}
                        required
                      />
                      <label htmlFor="hospital-area">Area *</label>
                    </div>
                  </div>

                  <div className="col-md-6">
                    <div className="form-floating">
                      <input
                        type="number"
                        className="form-control"
                        id="hospital-beds"
                        value={form.beds}
                        onChange={handleChange}
                        min="1"
                        required
                      />
                      <label htmlFor="hospital-beds">Number of Beds *</label>
                    </div>
                  </div>

                  <div className="col-md-6">
                    <div className="form-floating">
                      <select
                        className="form-select"
                        id="hospital-rating"
                        value={form.rating}
                        onChange={handleChange}
                        required
                      >
                        <option value="">Select Rating</option>
                        {[1, 2, 3, 4, 5].map((r) => (
                          <option key={r} value={r}>{r}</option>
                        ))}
                      </select>
                      <label htmlFor="hospital-rating">Rating (1–5) *</label>
                    </div>
                  </div>

                  <div className="col-12">
                    <div className="form-floating">
                      <input
                        type="text"
                        className="form-control"
                        id="hospital-specialization"
                        placeholder="e.g. Cardiology, Ortho"
                        value={form.specialization}
                        onChange={handleChange}
                        required
                      />
                      <label htmlFor="hospital-specialization">Specializations *</label>
                    </div>
                  </div>
                </div>

                <div className="d-flex gap-3 mt-4">
                  <button type="submit" className="btn btn-primary px-4 shadow-sm" disabled={loading}>
                    {loading ? 'Saving...' : 'Save Hospital'}
                  </button>
                  <button
                    type="button"
                    className="btn btn-outline-secondary px-4"
                    onClick={() => router.push('/AdminDashboard')}
                  >
                    Cancel
                  </button>
                </div>
              </form>

            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
